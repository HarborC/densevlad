function done = densevlad_dir_par(dirName, saveFileName)
% Outputs densevlads to a file using parallel computing
% Parameters: 
% dirName = directory to read image files from and convert to densevlads
% add label saving to the end of densevlad descriptor
%p = parpool(feature('numcores'));
%p = parpool(4);
files = dir(dirName); % './data/images/train'

x = {files.name};
files = files(~strcmp(x,'.') & ~strcmp(x,'..'));

spacing = (length(files))/10;
disp(spacing);
disp(length(files));

parfor i=1:10
    list_to_vlads(dirName, files(((i-1)*spacing)+1:(i*spacing)), [saveFileName '_' num2str(i)]);
end

done = true;
%delete(p);
end

