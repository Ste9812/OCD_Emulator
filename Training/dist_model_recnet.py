import CoreAudioML.miscfuncs as miscfuncs
import numpy as np
import random
import CoreAudioML.training as training
import CoreAudioML.dataset as CAMLdataset
import CoreAudioML.networks as networks
import torch
import torch.optim as optim
from torch.utils.tensorboard import SummaryWriter
import argparse
import time
import os
import csv
from scipy.io.wavfile import write

def init_model(save_path, args):
    if miscfuncs.file_check('model.json', save_path) and args.load_model:
        print('existing model file found, loading network')
        model_data = miscfuncs.json_load('model', save_path)
        try:
            assert model_data['model_data']['unit_type'] == args.unit_type
            assert model_data['model_data']['input_size'] == args.input_size
            assert model_data['model_data']['hidden_size'] == args.hidden_size
            assert model_data['model_data']['output_size'] == args.output_size
        except AssertionError:
            print("model file found with network structure not matching config file structure")
        network = networks.load_model(model_data)
    else:
        print('no saved model found, creating new network')
        network = networks.SimpleRNN(input_size=args.input_size, unit_type=args.unit_type, hidden_size=args.hidden_size,
                                     output_size=args.output_size, skip=args.skip_con)
        network.save_state = False
        network.save_model('model', save_path)
    return network

def main(args):
    start_time = time.time()
    if args.load_config:
        configs = miscfuncs.json_load(args.load_config, args.config_location)
        for parameters in configs:
            args.__setattr__(parameters, configs[parameters])
    if args.model == 'SimpleRNN':
        model_name = args.model + args.device + '_' + args.unit_type + '_hs' + str(args.hidden_size) + '_pre_' + args.pre_filt
    if args.pre_filt == 'A-Weighting':
        with open('Configs/' + 'b_Awght_mk2.csv') as csvfile:
            reader = csv.reader(csvfile, delimiter=',')
            args.pre_filt = list(reader)
            args.pre_filt = args.pre_filt[0]
            for item in range(len(args.pre_filt)):
                args.pre_filt[item] = float(args.pre_filt[item])
    elif args.pre_filt == 'high_pass':
        args.pre_filt = [-0.85, 1]
    elif args.pre_filt == 'None':
        args.pre_filt = None
    save_path = os.path.join(args.save_location, args.device + '-' + args.load_config)
    network = init_model(save_path, args)
    if not torch.cuda.is_available() or args.cuda == 0:
        print('cuda device not available/not selected')
        cuda = 0
    else:
        torch.set_default_tensor_type('torch.cuda.FloatTensor')
        torch.cuda.set_device(0)
        print('cuda device available')
        network = network.cuda()
        cuda = 1
    optimiser = torch.optim.Adam(network.parameters(), lr=args.learn_rate, weight_decay=1e-4)
    scheduler = optim.lr_scheduler.ReduceLROnPlateau(optimiser, 'min', factor=0.5, patience=5, verbose=True)
    loss_functions = training.LossWrapper(args.loss_fcns, args.pre_filt)
    train_track = training.TrainTrack()
    writer = SummaryWriter(os.path.join('TensorboardData', model_name))
    dataset = CAMLdataset.DataSet(data_dir='Data')
    dataset.create_subset('train', frame_len=22050)
    dataset.load_file(os.path.join('train', args.file_name), 'train')
    dataset.create_subset('val')
    dataset.load_file(os.path.join('val', args.file_name), 'val')
    init_time = time.time() - start_time + train_track['total_time']*3600
    network.save_state = True
    patience_counter = 0
    for epoch in range(train_track['current_epoch'] + 1, args.epochs + 1):
        print("Epoch: ", epoch)
        ep_st_time = time.time()
        epoch_loss = network.train_epoch(dataset.subsets['train'].data['input'][0],
                                         dataset.subsets['train'].data['target'][0],
                                         loss_functions, optimiser, args.batch_size, args.init_len, args.up_fr)
        writer.add_scalar('Time/EpochTrainingTime', time.time()-ep_st_time, epoch)
        if epoch % args.validation_f == 0:
            val_ep_st_time = time.time()
            val_output, val_loss = network.process_data(dataset.subsets['val'].data['input'][0],
                                             dataset.subsets['val'].data['target'][0], loss_functions, args.val_chunk)
            scheduler.step(val_loss)
            print("Val loss:", val_loss)
            if val_loss < train_track['best_val_loss']:
                patience_counter = 0
                network.save_model('model_best', save_path)
                write(os.path.join(save_path, "best_val_out.wav"),
                      dataset.subsets['val'].fs, val_output.cpu().numpy()[:, 0, 0])
            else:
                patience_counter += 1
            train_track.val_epoch_update(val_loss.item(), val_ep_st_time, time.time())
            writer.add_scalar('TrainingAndValidation/ValidationLoss', train_track['validation_losses'][-1], epoch)
        print('current learning rate: ' + str(optimiser.param_groups[0]['lr']))
        train_track.train_epoch_update(epoch_loss.item(), ep_st_time, time.time(), init_time, epoch)
        writer.add_scalar('TrainingAndValidation/TrainingLoss', train_track['training_losses'][-1], epoch)
        writer.add_scalar('TrainingAndValidation/LearningRate', optimiser.param_groups[0]['lr'], epoch)
        network.save_model('model', save_path)
        miscfuncs.json_save(train_track, 'training_stats', save_path)
        if args.validation_p and patience_counter > args.validation_p:
            print('validation patience limit reached at epoch ' + str(epoch))
            break
    del dataset
    dataset = CAMLdataset.DataSet(data_dir='Data')
    dataset.create_subset('test')
    dataset.load_file(os.path.join('test', args.file_name), 'test')
    print("Done Training")
    lossESR = training.ESRLoss()
    lossDC = training.DCLoss()
    print("Testing the final Model")
    test_output, test_loss = network.process_data(dataset.subsets['test'].data['input'][0],
                                                 dataset.subsets['test'].data['target'][0], loss_functions, args.test_chunk)
    test_loss_ESR = lossESR(test_output, dataset.subsets['test'].data['target'][0])
    test_loss_DC = lossDC(test_output, dataset.subsets['test'].data['target'][0])
    write(os.path.join(save_path, "test_out_final.wav"), dataset.subsets['test'].fs, test_output.cpu().numpy()[:, 0, 0])
    writer.add_scalar('Testing/FinalTestLoss', test_loss.item())
    writer.add_scalar('Testing/FinalTestESR', test_loss_ESR.item())
    writer.add_scalar('Testing/FinalTestDC', test_loss_DC.item())
    train_track['test_loss_final'] = test_loss.item()
    train_track['test_lossESR_final'] = test_loss_ESR.item()
    print("Testing the best Model")
    best_val_net = miscfuncs.json_load('model_best', save_path)
    network = networks.load_model(best_val_net)
    test_output, test_loss = network.process_data(dataset.subsets['test'].data['input'][0],
                                                 dataset.subsets['test'].data['target'][0], loss_functions, args.test_chunk)
    test_loss_ESR = lossESR(test_output, dataset.subsets['test'].data['target'][0])
    test_loss_DC = lossDC(test_output, dataset.subsets['test'].data['target'][0])
    write(os.path.join(save_path, "test_out_best.wav"),
         dataset.subsets['test'].fs, test_output.cpu().numpy()[:, 0, 0])
    writer.add_scalar('Testing/BestTestLoss', test_loss.item())
    writer.add_scalar('Testing/BestTestESR', test_loss_ESR.item())
    writer.add_scalar('Testing/BestTestDC', test_loss_DC.item())
    train_track['test_loss_best'] = test_loss.item()
    train_track['test_lossESR_best'] = test_loss_ESR.item()
    print("Finished Training: " + model_name)
    miscfuncs.json_save(train_track, 'training_stats', save_path)
    if cuda:
        with open(os.path.join(save_path, 'maxmemusage.txt'), 'w') as f:
            f.write(str(torch.cuda.max_memory_allocated()))

if __name__ == "__main__":
    prsr = argparse.ArgumentParser(
        description='''This script implements training for neural network amplifier/distortion effects modelling. This is
        intended to recreate the training of models of the ht1 amplifier and big muff distortion pedal, but can easily be 
        adapted to use any dataset''')
    prsr.add_argument('--device', '-p', default='ht1', help='This label describes what device is being modelled')
    prsr.add_argument('--data_location', '-dl', default='..', help='Location of the "Data" directory')
    prsr.add_argument('--file_name', '-fn', default='ht1',
                      help='The filename of the wav file to be loaded as the input/target data, the script looks for files'
                           'with the filename and the extensions -input.wav and -target.wav ')
    prsr.add_argument('--load_config', '-l',
                      help="File path, to a JSON config file, arguments listed in the config file will replace the defaults"
                      , default='RNN3')
    prsr.add_argument('--config_location', '-cl', default='Configs', help='Location of the "Configs" directory')
    prsr.add_argument('--save_location', '-sloc', default='Results', help='Directory where trained models will be saved')
    prsr.add_argument('--load_model', '-lm', default=True, help='load a pretrained model if it is found')
    prsr.add_argument('--seed', default=None, type=int, help='seed all of the random number generators if desired')
    prsr.add_argument('--segment_length', '-slen', type=int, default=22050, help='Training audio segment length in samples')
    prsr.add_argument('--epochs', '-eps', type=int, default=2000, help='Max number of training epochs to run')
    prsr.add_argument('--validation_f', '-vfr', type=int, default=2, help='Validation Frequency (in epochs)')
    prsr.add_argument('--validation_p', '-vp', type=int, default=25,
                      help='How many validations without improvement before stopping training, None for no early stopping')
    prsr.add_argument('--batch_size', '-bs', type=int, default=50, help='Training mini-batch size')
    prsr.add_argument('--iter_num', '-it', type=int, default=None,
                      help='Overrides --batch_size and instead sets the batch_size so that a total of --iter_num batches'
                           'are processed in each epoch')
    prsr.add_argument('--learn_rate', '-lr', type=float, default=0.005, help='Initial learning rate')
    prsr.add_argument('--init_len', '-il', type=int, default=200,
                      help='Number of sequence samples to process before starting weight updates')
    prsr.add_argument('--up_fr', '-uf', type=int, default=1000,
                      help='For recurrent models, number of samples to run in between updating network weights, i.e the '
                           'default argument updates every 1000 samples')
    prsr.add_argument('--cuda', '-cu', default=1, help='Use GPU if available')
    prsr.add_argument('--loss_fcns', '-lf', default={'ESRPre': 0.75, 'DC': 0.25},
                      help='Which loss functions, ESR, ESRPre, DC. Argument is a dictionary with each key representing a'
                           'loss function name and the corresponding value being the multiplication factor applied to that'
                           'loss function, used to control the contribution of each loss function to the overall loss ')
    prsr.add_argument('--pre_filt',   '-pf',   default='high_pass',
                        help='FIR filter coefficients for pre-emphasis filter, can also read in a csv file')
    prsr.add_argument('--val_chunk', '-vs', type=int, default=100000, help='Number of sequence samples to process'
                                                                                   'in each chunk of validation ')
    prsr.add_argument('--test_chunk', '-tc', type=int, default=100000, help='Number of sequence samples to process'
                                                                                   'in each chunk of validation ')
    prsr.add_argument('--model', '-m', default='SimpleRNN', type=str, help='model architecture')
    prsr.add_argument('--input_size', '-is', default=1, type=int, help='1 for mono input data, 2 for stereo, etc ')
    prsr.add_argument('--output_size', '-os', default=1, type=int, help='1 for mono output data, 2 for stereo, etc ')
    prsr.add_argument('--num_blocks', '-nb', default=1, type=int, help='Number of recurrent blocks')
    prsr.add_argument('--hidden_size', '-hs', default=16, type=int, help='Recurrent unit hidden state size')
    prsr.add_argument('--unit_type', '-ut', default='LSTM', help='LSTM or GRU or RNN')
    prsr.add_argument('--skip_con', '-sc', default=1, help='is there a skip connection for the input to the output')
    args = prsr.parse_args()
    if args.seed:
        torch.manual_seed(args.seed)
        np.random.seed(args.seed)
        random.seed(args.seed)
    main(args)