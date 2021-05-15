channels = 1
sr = 22050
input_dim = 60
dtw_cost = 'euclidean'
dtw_k = 1.1

patterns_rel_path = "patterns"
models_rel_path = "model"
preprocessed_rel_path = "preprocessed"
pattern_path_suffix = "_pattern"
recording_extension = ".raw"
recordings_glob = "*" + recording_extension
model_extension = ".npy"
models_glob = "*" + model_extension
active_path = 'active.txt'
