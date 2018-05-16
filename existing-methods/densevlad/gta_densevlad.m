% parameters:
% fName = name of image file to convert to densevlad
function v = gta_densevlad(fName)

%--- compute intra-normalized VLAD from dense SIFT 
%imfn = './data/example_gsv/L-NLvGeZ6JHX6JO8Xnf_BA_012_000.jpg'; % input image
imfn = fName; % input image
dictfn = './data/dnscnt_RDSIFT_K128.mat'; % pre-trained visual vocabulary 
vlad = at_image2densevlad(imfn,dictfn);   % compute dense-SIFT, then intra-norm VLAD

%--- PCA compression
pcafn = './data/dnscnt_RDSIFT_K128_vlad_pcaproj.mat'; % pre-computed PCA matrix
load(pcafn,'vlad_proj','vlad_lambda');
  
vladdim = 4096;
vlad_proj = single(vlad_proj(:,1:vladdim)');
vlad_wht = diag(1./sqrt(vlad_lambda(1:vladdim)));
  
v = single(yael_vecs_normalize(vlad_wht * (vlad_proj * vlad))); % PCA compression with whitening
end
