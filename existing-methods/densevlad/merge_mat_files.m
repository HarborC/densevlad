function done = merge_mat_files( dirName, saveFileName )

files = dir(dirName);

x = {files.name};
files = files(~strcmp(x,'.') & ~strcmp(x,'..'));

v = [];

for i=1:length(files)
    load([dirName '/' files(i).name],'vlads');
    v = [v; vlads];
end

save([saveFileName '.mat'],'v');
done = true;
end

