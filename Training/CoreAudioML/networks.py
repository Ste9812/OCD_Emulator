import torch
import torch.nn as nn
import CoreAudioML.miscfuncs as miscfuncs
import math
from contextlib import nullcontext

def wrapperkwargs(func, kwargs):
    return func(**kwargs)

def wrapperargs(func, args):
    return func(*args)

class SimpleRNN(nn.Module):

    def __init__(self, input_size=1, output_size=1, unit_type="LSTM", hidden_size=32, skip=1, bias_fl=True,
                 num_layers=1):
        super(SimpleRNN, self).__init__()
        self.input_size = input_size
        self.output_size = output_size
        self.rec = wrapperargs(getattr(nn, unit_type), [input_size, hidden_size, num_layers]) 
        self.lin = nn.Linear(hidden_size, output_size, bias=bias_fl)
        self.bias_fl = bias_fl
        self.skip = skip
        self.save_state = True
        self.hidden = None

    def forward(self, x):
        if self.skip:
            res = x[:, :, 0:self.skip]
            x, self.hidden = self.rec(x, self.hidden)
            return self.lin(x) + res
        else:
            x, self.hidden = self.rec(x, self.hidden)
            return self.lin(x)

    def detach_hidden(self):
        if self.hidden.__class__ == tuple:
            self.hidden = tuple([h.clone().detach() for h in self.hidden])
        else:
            self.hidden = self.hidden.clone().detach()

    def reset_hidden(self):
        self.hidden = None

    def save_model(self, file_name, direc=''):
        if direc:
            miscfuncs.dir_check(direc)
        model_data = {'model_data': {'model': 'SimpleRNN', 'input_size': self.rec.input_size, 'skip': self.skip,
                                     'output_size': self.lin.out_features, 'unit_type': self.rec._get_name(),
                                     'num_layers': self.rec.num_layers, 'hidden_size': self.rec.hidden_size,
                                     'bias_fl': self.bias_fl}}
        if self.save_state:
            model_state = self.state_dict()
            for each in model_state:
                model_state[each] = model_state[each].tolist()
            model_data['state_dict'] = model_state
        miscfuncs.json_save(model_data, file_name, direc)

    def train_epoch(self, input_data, target_data, loss_fcn, optim, bs, init_len=200, up_fr=1000):
        shuffle = torch.randperm(input_data.shape[1])
        ep_loss = 0
        for batch_i in range(math.ceil(shuffle.shape[0] / bs)):
            input_batch = input_data[:, shuffle[batch_i * bs:(batch_i + 1) * bs], :]
            target_batch = target_data[:, shuffle[batch_i * bs:(batch_i + 1) * bs], :]
            self(input_batch[0:init_len, :, :])
            self.zero_grad()
            start_i = init_len
            batch_loss = 0
            for k in range(math.ceil((input_batch.shape[0] - init_len) / up_fr)):
                output = self(input_batch[start_i:start_i + up_fr, :, :])
                loss = loss_fcn(output, target_batch[start_i:start_i + up_fr, :, :])
                loss.backward()
                optim.step()
                self.detach_hidden()
                self.zero_grad()
                start_i += up_fr
                batch_loss += loss
            ep_loss += batch_loss / (k + 1)
            self.reset_hidden()
        return ep_loss / (batch_i + 1)

    def process_data(self, input_data, target_data, loss_fcn, chunk, grad=False):
        with (torch.no_grad() if not grad else nullcontext()):
            output = torch.empty_like(target_data)
            for l in range(int(output.size()[0] / chunk)):
                output[l * chunk:(l + 1) * chunk] = self(input_data[l * chunk:(l + 1) * chunk])
                self.detach_hidden()
            if not (output.size()[0] / chunk).is_integer():
                output[(l + 1) * chunk:-1] = self(input_data[(l + 1) * chunk:-1])
            self.reset_hidden()
            loss = loss_fcn(output, target_data)
        return output, loss

class RecNet(nn.Module):

    def __init__(self, blocks=None, skip=0):
        super(RecNet, self).__init__()
        if type(blocks) == dict:
            blocks = [blocks]
        self.layers = nn.Sequential()
        self.block_types = {}
        self.block_types.update(dict.fromkeys(['RNN', 'LSTM', 'GRU'], BasicRNNBlock))
        self.skip = skip
        self.save_state = False
        self.input_size = None
        self.training_info = {'current_epoch': 0, 'training_losses': [], 'validation_losses': [],
                              'train_epoch_av': 0.0, 'val_epoch_av': 0.0, 'total_time': 0.0, 'best_val_loss': 1e12}
        try:
            for each in blocks:
                self.add_layer(each)
        except TypeError:
            print('no blocks provided, add blocks to the network via the add_layer method')

    def forward(self, x):
        if not self.skip:
            return self.layers(x)
        else:
            res = x[:, :, 0:self.skip]
            return self.layers(x) + res

    def detach_hidden(self):
        for each in self.layers:
            each.detach_hidden()

    def reset_hidden(self):
        for each in self.layers:
            each.reset_hidden()

    def add_layer(self, params):
        if self.input_size:
            pass
        else:
            self.input_size = params['input_size']
        self.layers.add_module('block_'+str(1 + len(list(self.layers.children()))),
                               self.block_types[params['block_type']](params))
        self.output_size = params['output_size']

    def save_model(self, file_name, direc=''):
        if direc:
            miscfuncs.dir_check(direc)
        model_data = {'model_data': {'model': 'RecNet', 'skip': 0}, 'blocks': {}}
        for i, each in enumerate(self.layers):
            model_data['blocks'][str(i)] = each.params
        if self.training_info:
            model_data['training_info'] = self.training_info
        if self.save_state:
            model_state = self.state_dict()
            for each in model_state:
                model_state[each] = model_state[each].tolist()
            model_data['state_dict'] = model_state
        miscfuncs.json_save(model_data, file_name, direc)


class BasicRNNBlock(nn.Module):

    def __init__(self, params):
        super(BasicRNNBlock, self).__init__()
        assert type(params['input_size']) == int, "an input_size of int type must be provided in 'params'"
        assert type(params['output_size']) == int, "an output_size of int type must be provided in 'params'"
        assert type(params['hidden_size']) == int, "an hidden_size of int type must be provided in 'params'"
        rec_params = {i: params[i] for i in params if i in ['input_size', 'hidden_size', 'num_layers']}
        self.params = params
        self.rec = wrapperkwargs(getattr(nn, params['block_type']), rec_params)
        self.lin_bias = params['lin_bias'] if 'lin_bias' in params else False
        self.lin = nn.Linear(params['hidden_size'], params['output_size'], self.lin_bias)
        self.hidden = None
        if 'skip' in params:
            self.skip = params['skip']
        else:
            self.skip = 1

    def forward(self, x):
        if self.skip:
            res = x[:, :, 0:self.skip]
            x, self.hidden = self.rec(x, self.hidden)
            return self.lin(x) + res
        else:
            x, self.hidden = self.rec(x, self.hidden)
            return self.lin(x)

    def detach_hidden(self):
        if self.hidden.__class__ == tuple:
            self.hidden = tuple([h.clone().detach() for h in self.hidden])
        else:
            self.hidden = self.hidden.clone().detach()

    def reset_hidden(self):
        self.hidden = None


def load_model(model_data):
    model_types = {'RecNet': RecNet, 'SimpleRNN': SimpleRNN}
    model_meta = model_data.pop('model_data')
    if model_meta['model'] == 'SimpleRNN':
        network = wrapperkwargs(model_types[model_meta.pop('model')], model_meta)
        if 'state_dict' in model_data:
            state_dict = network.state_dict()
            for each in model_data['state_dict']:
                state_dict[each] = torch.tensor(model_data['state_dict'][each])
            network.load_state_dict(state_dict)
    elif model_meta['model'] == 'RecNet':
        model_meta['blocks'] = []
        network = wrapperkwargs(model_types[model_meta.pop('model')], model_meta)
        for i in range(len(model_data['blocks'])):
            network.add_layer(model_data['blocks'][str(i)])
        if 'state_dict' in model_data:
            state_dict = network.state_dict()
            for each in model_data['state_dict']:
                state_dict[each] = torch.tensor(model_data['state_dict'][each])
            network.load_state_dict(state_dict)
        if 'training_info' in model_data.keys():
            network.training_info = model_data['training_info']
    return network

def legacy_load(legacy_data):
    if legacy_data['unit_type'] == 'GRU' or legacy_data['unit_type'] == 'LSTM':
        model_data = {'model_data': {'model': 'RecNet', 'skip': 0}, 'blocks': {}}
        model_data['blocks']['0'] = {'block_type': legacy_data['unit_type'], 'input_size': legacy_data['in_size'],
                                     'hidden_size': legacy_data['hidden_size'],'output_size': 1, 'lin_bias': True}
        if legacy_data['cur_epoch']:
            training_info = {'current_epoch': legacy_data['cur_epoch'], 'training_losses': legacy_data['tloss_list'],
                             'val_losses': legacy_data['vloss_list'], 'load_config': legacy_data['load_config'],
                             'low_pass': legacy_data['low_pass'], 'val_freq': legacy_data['val_freq'],
                             'device': legacy_data['pedal'], 'seg_length': legacy_data['seg_len'],
                             'learning_rate': legacy_data['learn_rate'], 'batch_size': legacy_data['batch_size'],
                             'loss_func': legacy_data['loss_fcn'], 'update_freq': legacy_data['up_fr'],
                             'init_length': legacy_data['init_len'], 'pre_filter': legacy_data['pre_filt']}
            model_data['training_info'] = training_info
        if 'state_dict' in legacy_data:
            state_dict = legacy_data['state_dict']
            state_dict = dict(state_dict)
            new_state_dict = {}
            for each in state_dict:
                new_name = each[0:7] + 'block_1.' + each[9:]
                new_state_dict[new_name] = state_dict[each]
            model_data['state_dict'] = new_state_dict
        return model_data
    else:
        print('format not recognised')
