fid = fopen('rb_sinc.c','w'); 

sincv = (-7:1/256:+8);
sincv = sinc(sincv);
sincv = sincv .* hamming(3841)';
%sincv(3841+1:3841+256) = 0;
fprintf(fid, "int32_t sinc[15][256] = {\n");

for i = 0:14
	fprintf(fid, "	{");
	for j = 1:256
		fprintf(fid, "%d", round(sincv(i*256 + j) * 65536));
		if ( j != 256) fprintf(fid, ", "); endif;
		if ( mod(j, 8) == 0 ) fprintf (fid, "\n	 "); endif;
	endfor
	if ( i != 14)
		fprintf(fid, "},\n");
	else
		fprintf(fid, "}\n");
	endif;
endfor
fprintf(fid, "};\n");

fclose(fid);
