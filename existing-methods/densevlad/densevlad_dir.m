% parameters: 
% dirName = directory to read image files from and convert to densevlads
% saveFileName = save densevlad vectors to this file
% add label saving with vectors
function done = densevlad_dir(dirName, saveFileName)
files=dir(dirName); % './data/images/train'
vlads = zeros(4096,3);

count = 1;

for k=1:5
   fileName= files(k).name;
   if ((~strcmp(fileName,'.')) && (~strcmp(fileName,'..')))
    path = [dirName '/' fileName];
    vlads(1:4096,count) = gta_densevlad(path);
    count = count + 1;
   end
end

save([saveFileName '.mat'],'vlads');

done = true;
end

