clc; close all; clear all;

filename1 = 'parallel_result.txt';
filename2 = 'serial_result.txt';

f1 = fopen(filename1);
f2 = fopen(filename2);ips


T1 = fscanf(f1,'%i %f\n');
T2 = fscanf(f2,'%i %f\n');

figure()
hold on; grid on;
xlabel('n'); ylabel('time, s')
plot(T1(1:2:end), T1(2:2:end), 'r', 'LineWidth',2)
plot(T2(1:2:end), T2(2:2:end), 'b', 'LineWidth',2)
legend('Parallel', 'Serial', 'Location','best')