program test_pas(input, output);
var a, b, c: integer;
begin
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
