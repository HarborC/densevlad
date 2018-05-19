% createns function requires an X which is nxk where n is num observations
% so we would have to transpose vlads matrix

load('train.mat');
train_vlads = v;
load('day1.mat');
test_vlads = v;

searcher = createns(train_vlads(:,1:4096),'Distance',@norm_dotproduct,'NSMethod','exhaustive');

indexofnn = knnsearch(searcher,test_vlads(:,1:4096),'K',50,'Distance',@norm_dotproduct);

relevantImages = 0;
currRecall = 0;

recalls = zeros(1,50);
precisions = zeros(1,50);

for i=1:length(indexofnn)
    relevantImages = 0;
    currRecall = 0;
    queried = train_vlads(indexofnn(i,:),4097) == test_vlads(i,4097);
    for j=1:50
        relevantImages = relevantImages + queried(j);
        currRecall = currRecall || queried(j);
        recalls(j) = recalls(j) + currRecall;
        precisions(j) = precisions(j) + (double(relevantImages)/double(j));
    end
    
end

recalls = recalls/length(indexofnn);
precisions = precisions/length(indexofnn);

save('results_day1.mat','recalls','precisions');
