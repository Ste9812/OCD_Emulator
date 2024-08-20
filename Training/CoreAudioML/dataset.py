import numpy as np
from scipy.io import wavfile
import torch
import math
import warnings
import os

def audio_converter(audio):
    if audio.dtype == 'int16':
        return audio.astype(np.float32, order='C') / 32768.0
    else:
        print('unimplemented audio data type conversion...')
        return audio.astype(np.float32)

def audio_splitter(audio, split_markers):
    assert sum(split_markers) <= 1.0
    if sum(split_markers) < 0.999:
        warnings.warn("sum of split markers is less than 1, so not all audio will be included in dataset")
    start = 0
    slices = []
    split_bounds = [int(x * audio.shape[0]) for x in split_markers]
    for n in split_bounds:
        end = start + n
        slices.append(audio[start:end])
        start = end
    return slices

def framify(audio, frame_len):
    audio = np.expand_dims(audio, 1) if len(audio.shape) == 1 else audio
    seg_num = math.floor(audio.shape[0] / frame_len) if frame_len else 1
    frame_len = audio.shape[0] if not frame_len else frame_len
    channels = audio.shape[1]
    dataset = torch.empty((frame_len, seg_num, channels))
    for i in range(seg_num):
        dataset[:, i, :] = torch.from_numpy(audio[i * frame_len:(i + 1) * frame_len, :])
    return dataset

class DataSet:

    def __init__(self, data_dir='../Dataset/', extensions=('input', 'target')):
        self.extensions = extensions if extensions else ['']
        self.subsets = {}
        assert type(data_dir) == str, "data_dir should be string,not %r" % {type(data_dir)}
        self.data_dir = data_dir

    def create_subset(self, name, frame_len=0):
        assert type(name) == str, "data subset name must be a string, not %r" %{type(name)}
        assert not (name in self.subsets), "subset %r already exists" %name
        self.subsets[name] = SubSet(frame_len)

    def load_file(self, filename, set_names='train', splits=None, cond_val=None):
        if type(set_names) == str:
            set_names = [set_names]
        assert len(set_names) == 1 or len(set_names) == len(splits), "number of subset names must equal number of " \
                                                                     "split markers"
        assert [self.subsets.get(each) for each in set_names], "set_names contains subsets that don't exist yet"
        for i, ext in enumerate(self.extensions):
            try:
                file_loc = os.path.join(self.data_dir, filename + '-' + ext)
                file_loc = file_loc + '.wav' if not file_loc.endswith('.wav') else file_loc
                np_data = wavfile.read(file_loc)
            except FileNotFoundError:
                file_loc = os.path.join(self.data_dir, filename + ext)
                file_loc = file_loc + '.wav' if not file_loc.endswith('.wav') else file_loc
                np_data = wavfile.read(file_loc)
            except FileNotFoundError:
                print(["File Not Found At: " + self.data_dir + filename])
                return
            raw_audio = audio_converter(np_data[1])
            if len(set_names) > 1:
                raw_audio = audio_splitter(raw_audio, splits)
                for n, sets in enumerate(set_names):
                    self.subsets[set_names[n]].add_data(np_data[0], raw_audio[n], ext, cond_val)
            elif len(set_names) == 1:
                self.subsets[set_names[0]].add_data(np_data[0], raw_audio, ext, cond_val)

class SubSet:

    def __init__(self, frame_len):
        self.data = {}
        self.cond_data = {}
        self.frame_len = frame_len
        self.conditioning = None
        self.fs = None

    def add_data(self, fs, audio, ext, cond_val):
        if not self.fs:
            self.fs = fs
        assert self.fs == fs, "data with different sample rate provided to subset"
        ext = 'data' if not ext else ext
        framed_data = framify(audio, self.frame_len)
        cond_data = cond_val * torch.ones(framed_data.shape[1]) if isinstance(cond_val, (float, int)) else None
        try:
            data = list(self.data[ext])
            self.data[ext] = (torch.cat((data[0], framed_data), 1),)
            if isinstance(cond_val, (float, int)):
                assert torch.is_tensor(self.cond_data[ext][0]), 'cond val provided, but previous data has no cond val'
                c_data = list(self.cond_data[ext])
                self.cond_data[ext] = (torch.cat((c_data[0], cond_data), 0),)
        except KeyError:
            self.data[ext] = (framed_data,)
            self.cond_data[ext] = (cond_data,)