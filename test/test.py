current_year=2022
name=input("Quel est ton nom ? ")
print("Bonjour "+ name +" !")
myList=[3, 5,len(name)]
print(len(myList))
if name == "Po Co" or False or name == "polnio":
  print("Bienvenue a l'admin "+ name)
while current_year > 2020:
  current_year=current_year - 1
print(current_year)
for i in range(1,11):
  print(i)
for i in range(1,current_year+1):
  print(i*2)
for element in myList:
  print(element)
