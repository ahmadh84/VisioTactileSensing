function deneme()
clear all;

% Initialize the constants
dt = 0.05;
dim = 17;
kL0 = 1.0;
c_dis = 0.5;                                    % The viscous damping coefficient
k = 100.0 * ones(dim * dim, dim * dim);			% The stiffness of the springs btw (i,j) and (k,l)
m = 1e1 * ones(dim * dim, 1);					% The masses at each point on the cloth

% Initialize the acceleration, velocity, force and position values
numTimeSteps = 100;
g = -9.81;
a = zeros(numTimeSteps, dim * dim, 3);
v = zeros(numTimeSteps, dim * dim, 3);
f = ones(numTimeSteps, dim * dim, 3);
f(1, :, :) = m * [0, 0, g];
p = zeros(numTimeSteps, dim * dim, 3);

% Initialize the connected springs - if 1, connected.
dirs = [[0, 1]; [1, 0]; [0, -1]; [-1, 0]; [1, 1]; [1, -1]; [-1, 1]; [-1, -1]; [0, 2]; [2, 0]; 
				[0, -2]; [-2, 0]];
lens = hypot(dirs(:,1), dirs(:,2));

% Initialize the positions
yDist = 20.0;
for i = 1 : dim
	for j = 1 : dim
		p(1, (j - 1) * dim + i, :) = [i * kL0, yDist, (dim - j) * kL0];
	end
end

% Update the positions 
for t = 2 : numTimeSteps
    disp(t);
	for i = 1 : dim
		for j = 1 : dim

			% %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%			
			% A. Update the acceleration, velocity and position with Euler rules

			% Update the acceleration
			idx = (j - 1) * dim + i;
			invm = (1 / m((j - 1) * dim + i));
			a(t, idx, :) = invm .* f(t-1, idx, :);

			% Update the velocity
			v(t, idx, :) = v(t-1, idx, :) + dt * a(t, idx, :);
%             assert(all(abs(v(t, idx, [1 2])) < 1e-5), 'woops');
            
			% Update the position
			p(t, idx, :) = p(t-1, idx, :) + dt * v(t, idx, :);

			% %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%			
			% B. Update the force

			% Compute the internal force due to springs
			f_int = zeros(1,1,3);
			for dir_idx = 1 : size(dirs, 1)

				% Get the neighbor and ensure that it is within limits.
				i2 = i + dirs(dir_idx, 1); j2 = j + dirs(dir_idx, 2);
				if((i2 < 1) || (j2 < 1) || (i2 > dim) || (j2 > dim))
                    continue;
                end
				idx2 = (j2 - 1) * dim + i2;

				% Compute the displacement
				currLength = p(t-1, idx2, :) - p(t-1, idx, :);
				normCurrLength = norm(squeeze(currLength(1:3)));
% 				if(normCurrLength == 0.0)
% 					p(t-1,idx,:)
% 					p(t-1,idx2,:)
% 					keyboard
% 					assert(false);
% 				end
				normLength = lens(dir_idx) * currLength / normCurrLength;
				displ = currLength - normLength;
				
				% Accumulate the internal force (note negative due to f = -kx)
				f_int = f_int - k(idx, idx2) * displ;
            end

            % Compute the force due to gravity
            f_g = reshape([0, 0, m(idx) * g], [1 1 3]);
            
			% Compute the viscous damping force
			f_dis = -c_dis * v(t, idx, :);

			% Compute some other force (no idea what it is)
			f_vi = 0.0;

			% Compute the total force
			f(t, idx, :) = (-f_int) + (f_g);
        end
    end

    p(t, 1:17, 1) = 1:17;
    p(t, 1:17, 2) = 20;
    p(t, 1:17, 3) = 16;
end

for t = 1 : numTimeSteps
	plot3(p(t,:,1), p(t,:,2), p(t,:,3), 'o', 'MarkerSize', 6);
    zlim([-70 25])
    xlabel('x');
    ylabel('y');
    zlabel('z');
	pause(0.22);
end
	
