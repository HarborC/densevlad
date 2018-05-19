function done = densevlad_dir(dirName, saveFileName)
% Outputs densevlads to a file. 
% Parameters: 
% dirName = directory to read image files from and convert to densevlads
% add label saving to the end of densevlad descriptor
files=dir(dirName); % './data/images/train'
vlads = zeros(4097,length(files)-2);

count = 1;

for k=1:length(files)
   fileName = files(k).name;
   if ((~strcmp(fileName,'.')) && (~strcmp(fileName,'..')))
    path = [dirName '/' fileName];
    vlads(1:4096,count) = gta_densevlad(path);
    % add label to end of vector
    vlads(4097,count) = str2double(fileName(1:find(fileName=='_')-1));
    count = count + 1
   end
end

vlads = vlads';

save([saveFileName '.mat'],'vlads');

done = true;
end

