clc;clear;
gaze_0 = textread('gaze_0.txt');
gaze_1 = textread('gaze_1.txt');
R = textread('R.txt');
[m,n]=size(gaze_0);
gaze_0_fullview = [];
gaze_1_fullview = [];
gaze_0_thetafai = [];
gaze_1_thetafai = [];

% gaze_0(:,3) = gaze_0(:,3).* -1;
% gaze_1(:,3) = gaze_1(:,3).* -1;
for i=1:m
    %gaze0
    abb = R * gaze_0(i,:)';
    gaze_0_fullview = [gaze_0_fullview;abb'];
    if (abb(2,1)<=0 && abb(1,1)<0)
        fai0 = 180 + atand(abb(2,1)/abb(1,1));
        theta0 = atand(sqrt(abb(2,1)*abb(2,1)+abb(1,1)*abb(1,1)) / abb(3,1));
    elseif (abb(2,1)<=0 && abb(1,1)>=0)
         fai0 = 360 + atand(abb(2,1)/abb(1,1));
         theta0 = atand(sqrt(abb(2,1)*abb(2,1)+abb(1,1)*abb(1,1)) / abb(3,1));
    elseif (abb(2,1)>=0 && abb(1,1)>=0)
         fai0 = atand(abb(2,1)/abb(1,1));
         theta0 = atand(sqrt(abb(2,1)*abb(2,1)+abb(1,1)*abb(1,1)) / abb(3,1));
    elseif (abb(2,1)>=0 && abb(1,1)<=0)
         fai0 = 180 + atand(abb(2,1)/abb(1,1));
         theta0 = atand(sqrt(abb(2,1)*abb(2,1)+abb(1,1)*abb(1,1)) / abb(3,1));
    end
    if (abb(2,1)==0 && abb(1,1)==0)
        fai0 = 0;
        theta0 = 0;
    end
    if theta0<0
        theta0 = -theta0;
    end
    
    gaze_0_thetafai = [gaze_0_thetafai;fai0,theta0];
    
    %gaze1
    acc = R * gaze_1(i,:)';
    gaze_1_fullview = [gaze_1_fullview;acc'];
    if (acc(2,1)<=0 && acc(1,1)<0)
        fai1 = 180 + atand(acc(2,1)/acc(1,1));
        theta1 = atand(sqrt(acc(2,1)*acc(2,1)+acc(1,1)*acc(1,1)) / acc(3,1));
    elseif (acc(2,1)<=0 && acc(1,1)>=0)
         fai1 = 360 + atand(acc(2,1)/acc(1,1));
         theta1 = atand(sqrt(acc(2,1)*acc(2,1)+acc(1,1)*acc(1,1)) / acc(3,1));
    elseif (acc(2,1)>=0 && acc(1,1)>=0)
         fai1 = atand(acc(2,1)/acc(1,1));
         theta1 = atand(sqrt(acc(2,1)*acc(2,1)+acc(1,1)*acc(1,1)) / acc(3,1));
    elseif (acc(2,1)>=0 && acc(1,1)<=0)
         fai1 = 180 + atand(acc(2,1)/acc(1,1));
         theta1 = atand(sqrt(acc(2,1)*acc(2,1)+acc(1,1)*acc(1,1)) / acc(3,1));
    end
    if (acc(2,1)==0 && acc(1,1)==0)
        fai1 = 0;
        theta1 = 0;
    end
    if theta1<0
        theta1 = -theta1;
    end
    
    gaze_1_thetafai = [gaze_1_thetafai;fai1,theta1];
end

gaze_5_thetafai = (gaze_1_thetafai + gaze_0_thetafai)/2;

% csvwrite('gaze_0_fullview.csv',gaze_0_fullview);
% csvwrite('gaze_1_fullview.csv',gaze_1_fullview);
% csvwrite('gaze_0_thetafai.csv',gaze_0_thetafai);
% csvwrite('gaze_1_thetafai.csv',gaze_1_thetafai);

fid=fopen('gaze_0_thetafai.txt','wt');%写入文件路径 
matrix=gaze_0_thetafai;                        %input_matrix为待输出矩阵 
[t,y]=size(matrix);  
for i=1:1:t    
    for j=1:1:y       
        if j==y          
            fprintf(fid,'%g\n',matrix(i,j));      
        else
            fprintf(fid,'%g\t',matrix(i,j));
        end
    end
end
fclose(fid);

fid=fopen('gaze_1_thetafai.txt','wt');%写入文件路径 
matrix=gaze_1_thetafai;                        %input_matrix为待输出矩阵 
[t,y]=size(matrix);  
for i=1:1:t    
    for j=1:1:y       
        if j==y          
            fprintf(fid,'%g\n',matrix(i,j));      
        else
            fprintf(fid,'%g\t',matrix(i,j));
        end
    end
end
fclose(fid);

fid=fopen('gaze_5_thetafai.txt','wt');%写入文件路径 
matrix=gaze_5_thetafai;                        %input_matrix为待输出矩阵 
[t,y]=size(matrix);  
for i=1:1:t    
    for j=1:1:y       
        if j==y          
            fprintf(fid,'%g\n',matrix(i,j));      
        else
            fprintf(fid,'%g\t',matrix(i,j));
        end
    end
end
fclose(fid);