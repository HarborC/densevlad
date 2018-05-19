% dataset: day1
load('results_day1.mat');
subplot(3,2,1);
plot(recalls,precisions);
title("Day1");
xlabel("Recall")
ylabel("Precision")

% dataset: day2
load('results_day2.mat');
subplot(3,2,2);
plot(recalls,precisions);
title("Day2");
xlabel("Recall")
ylabel("Precision")

% dataset: afternoon
load('results_afternoon.mat');
subplot(3,2,3);
plot(recalls,precisions);
title("Afternoon");
xlabel("Recall")
ylabel("Precision")

% dataset: evening
load('results_evening.mat');
subplot(3,2,4);
plot(recalls,precisions);
title("Evening");
xlabel("Recall")
ylabel("Precision")

% dataset: night
load('results_night.mat');
subplot(3,2,5);
plot(recalls,precisions);
title("Night");
xlabel("Recall")
ylabel("Precision")
