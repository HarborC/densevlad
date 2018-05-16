files=dir('./data/images/train');
train_vlads = [];


for k=1:5
   fileName= files(k).name;
   if ((~strcmp(fileName,'.')) && (~strcmp(fileName,'..')))
    path = ['./data/images/train' '/' fileName]
    train_vlads = [train_vlads gta_densevlad(path)];
   end
end
save('train.mat','train_vlads');