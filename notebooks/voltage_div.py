



R1s = [4.7,1,10,100,47]
Rfs = list(R1s)

for R1 in R1s:
    for R2 in Rfs:


        RD = R1/(R1+R2)*5
        print(f'R1: {R1} R2: {R2} VOUT: {RD}')

f = input()