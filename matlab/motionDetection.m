function [out, accel] = motionDetection( x_accel, y_accel, thresh)
% Usage:
%   out = motionDetection( x_accel, y_accel, thresh)
% Description:
%  determines motion by compmuting the magnitude of the acceleration vector
%  and comparing the result to a threshold.  returns a vector of ones and
%  zeros to indicate motion
% Inputs:
%  x_accel - acceleration in the x - axis
%  y_accel - accleration in the y - axis
% Outpus:
% out - array of ones anz zeros used to indicat notion

accel = sqrt((x_accel.^2 + y_accel.^2 ));

out = zeros(1,length(accel));

ind = find( accel > thresh);
out(ind) = 1;