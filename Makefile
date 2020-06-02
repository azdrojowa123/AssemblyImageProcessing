all: png

png: png.c pngNew.s licznik.s
        gcc -m32 -O3  -Ipng   png.c pngNew.s licznik.s -lpng  -o png  


clean:
        rm png 


