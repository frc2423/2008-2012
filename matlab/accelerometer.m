FLEN = 64;


% set filtering time constant
alpha = .05;
a=[1 -(1-alpha)];
b=[alpha];

x_accel=readBinFile(fnamex,'float','ieee-be');
% filter the x acceleration and subtract the mean
mx=mean(x_accel(150:end));
xf=filter(b,a,x_accel-mx);

y_accel=readBinFile(fnamey,'float','ieee-be');
% filter the y acceleration and subtract the mean
my=mean(y_accel(150:end));
yf=filter(b,a,y_accel-my);

%set the the delta time
dt=.1; %100ms
N=length(x_accel);
t=(0:N-1)*dt;

%initialize the integration variables
accumx = 0; 
x_velocity=zeros(1,length(yf));
accumxx = 0;
x_position=zeros(1,length(yf));

accumy = 0;
y_velocity=zeros(1,length(yf));
accumyy = 0;
y_position=zeros(1,length(yf));


%experimental -- probly. don't need this
K = 1; % damping term

% determine motion based on the magnitude of the filtered accleration
[motionFlag, accel] = motionDetection( xf, yf, .2);


% integrate over time to get velocity and position
% in x and y
for ind=1:length(yf)
    
    if( motionFlag(ind) )
        accumy = accumy + K*yf(ind)*dt;
        accumyy = accumyy + K*accumy*dt;
    
        accumx = accumx + K*xf(ind)*dt;
        accumxx = accumxx + K*accumx*dt;   
    end
        
 
    if( ind > 5)  
        if( sum(motionFlag(ind-5:ind)) == 0)
          sum(motionFlag(ind-5:ind))
          accumx = 0;
          accumy = 0;
        end
    end
        
    
    
    x_velocity(ind)=accumx;
    x_position(ind) = accumxx;
    
 
    y_velocity(ind)=accumy;
    y_position(ind) = accumyy;

    
end



figure(1);
plot(t,x_accel,t,x_velocity,'r',t,x_position,'g');
grid on;  
%Legend('Acc','Vel','Pos');
title('X Axis Accelerometer');
figure(2);
plot(t,y_accel,t,y_velocity,'r',t,y_position,'g');
grid on;
%Legend('Acc','Vel','Pos');
title('Y Axis Accelerometer');



