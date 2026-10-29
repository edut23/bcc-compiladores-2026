program testpas(input, output);
var a, b: integer;
begin
	if a = b then
		a := a + b
	else
		b := b - a;
	while a + b >= 0 do
		a := -b
end.

{Tudo que vier depois do ponto é comentário}