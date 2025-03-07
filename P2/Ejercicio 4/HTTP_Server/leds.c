/*OPERADOR AND:

Hacer resultado=leds&0x02 implica que para que resultado tome valor exacto de 0x02 es necesario que leds=0x02
Esto es debido a que la operacion (&) hace una comparativa bit a bit de ambas variables a comparar y solo devuelve
como '1' aquellos dos bits comparados que también son '1':

Ejemplo 1: 1110(14) & 0001(1)= 0000(0) -> ningun bit a '1' coincide luego es 0
Ejemplo 2:1111(15) & 0111(7) = 0111(7) -> los 3 bits menos significativos coinciden a '1' luego es 7

Una vez entendido esto se comprende mejor porque en la funcion salida_leds se hace leds&0x02, leds&0x04 etc..
Esto es debido a que se pretende comparar la variable P2 escrito en el fichero HTTP_Server_CGI que se pasa por 
parametro a la funcion salida_leds con los numero binarios 0x02, 0x04 y 0x08 de modo que en los casos en los 
que el bit correspondiente coincida, el resultado de leds&Hexadecimal será distinto de 
cero(coincidira justo un solo bit)y por tanto habrá un nivel alto el writepin que encenderá los leds.

OPERADOR OR

Hacer resultado=rgb|0x02 implica que para que resultado tome valor exacto de 0x02 es necesario que leds=0x00
Esto es debido a que la operacion (|) hace una comparativa bit a bit de ambas variables a comparar y devuelve 
un '1' binario si uno de los dos operandos tiene un '1' en ese bit, aunque el otro tenga un 0
Ejemplo 1: 1110(14) | 0001(1)= 1111(15) -> el primer numero tiene 3 unos y el segundo el ultimo 1, luego es 15(1111)
Ejemplo 2:0101(5) & 1101(13) = 1101(13) -> hay '1' en todos los bits menos en el 2º bit menos significativo,luego  13

Una vez entendido esto, se comprende mejor porque en la funcion salida_leds se hace rgb|0xEF,rgb|0xDF,rgb|0xBF.
Esto es debido a que se quiere hacer una operacion OR entre la variable rgb escrita en el ficher HTTP_Server_CGI que
se pasa por parametro a la funcion salida_leds con los numero binarios 0xEF, 0xDF, 0xBF de modo que, si yo tengo
un valor de rgb de 0x10 para led4(me conviene tener binarios donde solo hay un bit a 1 para manejar los checkboxes de mi 
pagina web) y le hago una OR con 0xEF obtengo: 0001 0000 | 1110 1111 = 1111 1111. Entonces obtengo todo unos pero
esto supone unos rgbs apagados por que están en anodo comun luego los enciendo con niveles bajos.De ahí la importancia
de (~) que complementa todos los bits, poniendolos a cero y teniendo un nivel bajo(es necesario que todos los bits 
esten a 0 para que haya un nivel bajo,no obstante, basta con un bit a 1 en un binario de 8 bits para tener nivel alto)
Lo mismo sucede cuando tengo rgb=0x20 para led5, donde hago 0x20|0xDF-> 0010 0000 | 1101 1111 =  1111 1111
Vuelvo a negar para tener todos los bits a '0' y tener un nivel bahjo que encienda mi rgb. El ultimo caso es analogo

Una vez explicado esto es necesario entender que para manejar los checkboxes se ha necesitado concatenar la variable
P2 con rg(P2|rgb) con el fin de crear una variable que abarque tanto leds normales como los del rgb, permitiendo
así un manejo viable de los checkboxes y permitiendo su correcto funcionamiento.
*/

#include "leds.h"

void Init_All_Leds(void){
  GPIO_InitTypeDef GPIO_InitStruct_Led={0};
  __HAL_RCC_GPIOB_CLK_ENABLE();//habilita el reloj del pueto B para poder funcionar los distintos GPIOs localizados en el mismo
  GPIO_InitStruct_Led.Mode=GPIO_MODE_OUTPUT_PP ; //modo push-pull indicado por el fabricante
  GPIO_InitStruct_Led.Pull=GPIO_PULLUP; // pullpup, indicado por fabricante
  GPIO_InitStruct_Led.Speed=GPIO_SPEED_FREQ_VERY_HIGH; // indicado por fabricante
  GPIO_InitStruct_Led.Pin=GPIO_PIN_0 |GPIO_PIN_7|GPIO_PIN_14; // indica que pines vamos a utilizar
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct_Led); // inicializa la estructura que prepara los leds
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0|GPIO_PIN_7|GPIO_PIN_14,GPIO_PIN_RESET);//Leds apagados inicialmente
  GPIO_InitTypeDef GPIO_InitStruct_Rgb={0};
__HAL_RCC_GPIOD_CLK_ENABLE();                                 //habilita el reloj del pueto D
  GPIO_InitStruct_Rgb.Mode=GPIO_MODE_OUTPUT_PP ;              //modo push-pull indicado por el fabricante
	GPIO_InitStruct_Rgb.Pull = GPIO_PULLDOWN;      // no es necesario pullup o pulldown pues push-pull ofrece niveles logicos 1 y 0
  GPIO_InitStruct_Rgb.Speed=GPIO_SPEED_FREQ_VERY_HIGH;        // indicado por fabricante
  GPIO_InitStruct_Rgb.Pin=GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13;// indica que pines vamos a utilizar
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct_Rgb);                 //Inicializa el GPIO con los parámtros indicados
  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13,GPIO_PIN_SET);//RGB apagado inicialmente
  }


void salida_leds(uint8_t leds,uint8_t rgb){
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0,leds&0x02);        //led1 verde
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7,leds&0x04);        //led2 azul
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14,leds&0x08);       //led3 rojo
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11,~(rgb|0xEF));     //blue RGB
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12,~(rgb|0xDF));     //green RGB
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13,~(rgb|0xBF));     //red RGB
 }

 void reset_leds(){
   
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
	//RGB LEDS
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, GPIO_PIN_SET);
 }



