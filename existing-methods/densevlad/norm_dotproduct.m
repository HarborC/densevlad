function dotproduct = norm_dotproduct(dv1, dv2)
%NORM_DOTPRODUCT calculates normalized dot product of two vectors
% normalizes two vectors and then calculates their dot products
% this is used as a scoring metric for custom densevlad descriptors
% where last element is the label (so that is ignored)

normdv1 = dv1(1:4096)/norm(dv1(1:4096),2);
normdv2 = dv2(1:4096)/norm(dv2(1:4096),2);

dotproduct = dot(normdv1,normdv2);

end

