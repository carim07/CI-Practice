texterior=load('Temperatura_exterior');
tinterior=load('Temperatura_interior');
tdeseada=load('Temperatura_deseada');
tf = length(texterior);
t=[0:tf-1];
subplot(2,2,1);
plot(t,texterior,t,tinterior,t,tdeseada);
title("Funcionamiento del sistema");
subplot(2,2,2);
plot(t,tdeseada-tinterior);
title("Entrada: Error TD - TI");

% Grafico de la curva de sistema
x=load('entrada');
curva=load('Curva_sistema');
lc=length(curva); %mismo largo que x
m=(lc/2)+1;
subplot(2,2,3);
plot(x(m:lc),curva(m-1:-1:1));
title("Respuesta del Sistema - Voltaje");
subplot(2,2,4);
plot(x(m:lc),curva(m:lc));
title("Respuesta del Sistema - Intensidad de Corriente");
