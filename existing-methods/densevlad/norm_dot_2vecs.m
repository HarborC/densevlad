function d = norm_dot_2vecs( dv1, dv2 )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
dv1 = dv1/norm(dv1,2);
dv2 = dv2/norm(dv2,2);
d = dot(dv1,dv2);

end

