# apollo2csv
Convert [Apollo Blastsimulator](https://www.emi.fraunhofer.de/en/service-offers/software-solutions/apollo.html) gauges file to csv format.

## Usage
```
apollo2csv -f file [-p -i -a -v]

  -f Apollo gauges file
  -p Pressure unit psi, default kPa
  -i Keep impulse, default drop
  -a Keep ambient pressure, default conpensate
  -v Apollo version 2018/2020, default 2020
```
## Examples
### Gauges file
```
#DATA WRITTEN BY APOLLO-BLASTSIMULATOR
#sampling rate:     1
#number of gauges:  3
#names of gauges: "G1" "G2" "G3"
#number of quantities:  3
#data structure: begin column, end column, labels: select, axis, key
#   1    1 "Time" "Time [s]" "t@"
#   2    4 "Pressure" "Pressure [Pa]" "P@"
#   5    7 "Impulse"  "Impulse [Pa s]" "i@"
  0.500383E-02  0.90033E+04  0.11977E+06  0.15961E+06  0.22167E+04  0.33228E+03  0.16202E+03
  0.502379E-02  0.89112E+04  0.11759E+06  0.15916E+06  0.22149E+04  0.33261E+03  0.16317E+03
  0.504365E-02  0.88210E+04  0.11553E+06  0.15868E+06  0.22130E+04  0.33289E+03  0.16430E+03
  0.506340E-02  0.87325E+04  0.11352E+06  0.15814E+06  0.22112E+04  0.33313E+03  0.16543E+03
  0.508318E-02  0.86451E+04  0.11159E+06  0.15748E+06  0.22094E+04  0.33333E+03  0.16654E+03
```
### ./apollo2csv -f gauges
```
Time [ms], G1 [kPa], G2 [kPa], G3 [kPa], 
5.00383000, 0.00, 110.77, 150.61, 
5.02379000, -0.09, 108.59, 150.16, 
5.04365000, -0.18, 106.53, 149.68, 
5.06340000, -0.27, 104.52, 149.14, 
5.08318000, -0.36, 102.59, 148.48, 

```
### ./apollo2csv -f gauges -p -i -a
```
Time [ms], G1 [psi], G2 [psi], G3 [psi], G1 [psi ms], G2 [psi ms], G3 [psi ms], 
5.00383000, 1.31, 17.37, 23.15, 321.51, 48.19, 23.50, 
5.02379000, 1.29, 17.05, 23.08, 321.24, 48.24, 23.67, 
5.04365000, 1.28, 16.76, 23.01, 320.97, 48.28, 23.83, 
5.06340000, 1.27, 16.46, 22.94, 320.71, 48.32, 23.99, 
5.08318000, 1.25, 16.18, 22.84, 320.45, 48.35, 24.15, 
```
## License
[MIT](https://choosealicense.com/licenses/mit/)
