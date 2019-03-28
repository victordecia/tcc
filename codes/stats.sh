#!/bin/bash
#echo "Recuperando estatísticas sobre os dados..."
#echo "Dataset GLASS"
#echo "Matriz Original:"
echo "GLASS"
for i in {1..10}
do
	./getResults < resultados/glass/LGC/M_0_$i.resp 
done
echo ""
for i in {1..10}
do
	./getResults < resultados/glass/LGC/M_M_3_$i.resp 
done
echo ""
for i in {1..10}
do
	./getResults < resultados/glass/LGC/M_M_2_$i.resp 
done
echo "\nIRIS"
for i in {1..10}
do
	./getResults < resultados/iris/LGC/M_0_$i.resp 
done
echo ""
for i in {1..10}
do
	./getResults < resultados/iris/LGC/M_M_3_$i.resp 
done
echo ""
for i in {1..10}
do
	./getResults < resultados/iris/LGC/M_M_2_$i.resp 
done
echo "\nPARKINSONS"
for i in {1..10}
do
	./getResults < resultados/parkinsons/LGC/M_0_$i.resp 
done
echo ""
for i in {1..10}
do
	./getResults < resultados/parkinsons/LGC/M_M_19_$i.resp 
done
echo ""
for i in {1..10}
do
	./getResults < resultados/parkinsons/LGC/M_M_5_$i.resp 
done
echo "\nSEEDS"
for i in {1..10}
do
	./getResults < resultados/seeds/LGC/M_0_$i.resp 
done
echo ""
for i in {1..10}
do
	./getResults < resultados/seeds/LGC/M_M_6_$i.resp 
done
echo ""
for i in {1..10}
do
	./getResults < resultados/seeds/LGC/M_M_5_$i.resp 
done
echo "\nWINE"
for i in {1..10}
do
	./getResults < resultados/wine/LGC/M_0_$i.resp 
done
echo ""
for i in {1..10}
do
	./getResults < resultados/wine/LGC/M_M_9_$i.resp 
done
echo ""
for i in {1..10}
do
	./getResults < resultados/wine/LGC/M_M_7_$i.resp 
done
