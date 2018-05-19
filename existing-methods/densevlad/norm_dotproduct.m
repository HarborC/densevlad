function dotproduct = norm_dotproduct(dv1, dv2)
%NORM_DOTPRODUCT calculates normalized dot product of two vectors
% normalizes two vectors and then calculates their dot products
% this is used as a scoring metric for custom densevlad descriptors
% where last element is the label (so that is ignored)

D2 = zeros(length(dv2),1);
[x,y] = size(dv2);
normdv1 = dv1/norm(dv1,2);
normdv2 = zeros(x,y);
for i=1:x
    normdv2(i,:) = dv2(i,:)/norm(dv2(i,:),2);
end

dotproduct = acos(normdv2*normdv1');

end

