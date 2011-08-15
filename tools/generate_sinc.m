fid = fopen('sinc.c','w');

sincv = (0:1/256:+8-1/256);
sincv = sinc(sincv);
ham = hamming(1793*2+256*2)';
sincv = sincv .* ham(257+7*256:size(ham)(2)-2);
%sincv(3841+1:3841+256) = 0;
fprintf(fid, "int32_t sinc[256][8] = {\n");

for j = 1:256
	fprintf(fid, "    {");
	for i = 0:7
		fprintf(fid, "%d", round(sincv(i*256 + j) * 65536));
		if ( i != 7) fprintf(fid, ", "); endif;
		%if ( mod(j, 8) == 0 ) fprintf (fid, "\n     "); endif;
	endfor

	if ( j != 256)
		fprintf(fid, "},\n");
	else
		fprintf(fid, "}\n");
	endif;

	%if ( mod(j, 2) == 0 ) fprintf (fid, "\n     "); endif;

endfor
fprintf(fid, "};\n");
fclose(fid);
