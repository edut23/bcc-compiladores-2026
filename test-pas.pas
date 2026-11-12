program test_pas{lexical level 1}(input, output);
	var a, b, c: integer;
	procedure proc_test_A{begin lexical level 2}(var a, b, c: integer; x, y: real);
		var u, v, w: real;
		procedure proc_test_B{begin lexical level 3}(c: integer);
			var x, y, z: real;
			begin
				x := c;
				y := a;
				z := b;
			end;{end proc_test_B}
		begin
		end;

	begin
		proc_test_A(a, b, u, v)
		if a = b then
			begin
				a := a + b + 3;
				if a > 0 then
					b := b + c - a
				else
					c := a * (b + c)
			end
		else
			b := b - a;
		while a + b >= 0 do
			a := -b
	end.

{Tudo que vier depois do ponto é comentário}
