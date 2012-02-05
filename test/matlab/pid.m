function out = pid( setpoint, actual_position, Kp, Ki, Kd, N, dt);

previous_error = 0;
integral = 0 ;

error = zeros(1,N);

ind = 0;
for ind=1:N
  error = setpoint(ind) - actual_position(ind);
  integral = integral + error*dt;
  derivative = (error - previous_error)/dt;
  output(ind) = Kp*error + Ki*integral + Kd*derivative;
  previous_error = error;
  wait(dt)
end
