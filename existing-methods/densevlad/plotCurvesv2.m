files = dir('results*.mat');

for i = 1:length(files)
    dataset = files(i).name;
    % dataset: night
    load(dataset);
    subplot(4,3,i);
    plot(recalls,precisions);
    title(dataset(9:end));
    xlabel("Recall")
    ylabel("Precision")
end
