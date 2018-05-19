function done = list_to_vlads(dirName, fileList, matFileName)
% LIST_TO_VLADS takes a list of files and converts them to densevlads
% then saves them to a .mat file

count = 1;
vlads = single(zeros(4097,length(fileList)));
for k=1:length(fileList)
   fileName = fileList(k).name;
   if ((~strcmp(fileName,'.')) && (~strcmp(fileName,'..')))
    path = [dirName '/' fileName];
    vlads(1:4096,count) = gta_densevlad(path);
    % add label to end of vector
    vlads(4097,count) = str2double(fileName(1:find(fileName=='_')-1));
    count = count + 1;
   end
end

vlads = vlads';

save([matFileName '.mat'],'vlads');

done = true;
end

