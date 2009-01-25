function data=readBinFile(fname,precision, endian);

if( nargin < 2 )
    precision = 'uint8';
end

if( nargin < 3 )
    endian = 'ieee-le' ;
end

fid = fopen(fname);
if fid == -1
    str = sprintf('Can`t open input file %s\n',fname);
    error(str);
end

data = fread( fid, inf, precision,endian);

fclose(fid);