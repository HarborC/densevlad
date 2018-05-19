% createns function requires an X which is nxk where n is num observations
% so we would have to transpose vlads matrix

load('train.mat');
train_vlads = v;
load('night.mat');
test_vlads = v;
topK = 50;

[x,y] = size(test_vlads);

indexofnn = zeros(x,topK);

for i=1:x
    prods(:,2) = norm_dotproduct(test_vlads(i,1:4096),train_vlads(:,1:4096));
    prods(:,1) = (1:1:1400);
    p = sortrows(prods,2);
    indexofnn(i,:) = p(1:50,1)';
end

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

save('results_night.mat','recalls','precisions');

