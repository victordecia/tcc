#!/bin/bash
LD_LIBRARY_PATH=/usr/local/lib:${LD_LIBRARY_PATH}
cd LGC/
echo "Processando LCG com knn variando de 1..10 para os datasets"
echo "Dataset GLASS ..."
for i in {1..10}
do
   ./LGC ../resultados/glass/M_0.data $i > ../resultados/glass/LGC/M_0_$i.resp
   ./LGC ../resultados/glass/M_M_2.data $i > ../resultados/glass/LGC/M_M_2_$i.resp
   ./LGC ../resultados/glass/M_M_3.data $i > ../resultados/glass/LGC/M_M_3_$i.resp
done
echo "Dataset IRIS ..."
for i in {1..10}
do
   ./LGC ../resultados/iris/M_0.data $i > ../resultados/iris/LGC/M_0_$i.resp
   ./LGC ../resultados/iris/M_M_2.data $i > ../resultados/iris/LGC/M_M_2_$i.resp
   ./LGC ../resultados/iris/M_M_3.data $i > ../resultados/iris/LGC/M_M_3_$i.resp
done
echo "Dataset PARKINSONS ..."
for i in {1..10}
do
   ./LGC ../resultados/parkinsons/M_0.data $i > ../resultados/parkinsons/LGC/M_0_$i.resp
   ./LGC ../resultados/parkinsons/M_M_5.data $i > ../resultados/parkinsons/LGC/M_M_5_$i.resp
   ./LGC ../resultados/parkinsons/M_M_19.data $i > ../resultados/parkinsons/LGC/M_M_19_$i.resp
done
echo "Dataset SEEDS ..."
for i in {1..10}
do
   ./LGC ../resultados/seeds/M_0.data $i > ../resultados/seeds/LGC/M_0_$i.resp
   ./LGC ../resultados/seeds/M_M_5.data $i > ../resultados/seeds/LGC/M_M_5_$i.resp
   ./LGC ../resultados/seeds/M_M_6.data $i > ../resultados/seeds/LGC/M_M_6_$i.resp
done
echo "Dataset WINE ..."
for i in {1..10}
do
   ./LGC ../resultados/wine/M_0.data $i > ../resultados/wine/LGC/M_0_$i.resp
   ./LGC ../resultados/wine/M_M_7.data $i > ../resultados/wine/LGC/M_M_7_$i.resp
   ./LGC ../resultados/wine/M_M_9.data $i > ../resultados/wine/LGC/M_M_9_$i.resp
done
