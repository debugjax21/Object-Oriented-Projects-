"""
This is an object-oriented design projcet that simulates a vending machine that dispenses different types
of (old-fashioned) coffee as well as chicken bouillon.
The program can either be ran from the shell by calling Main() or through the command line.

Things I learned: I learned more about classes, how they work and how I can fit them together to make a whole,
a.k.a the coffee machine. I also learned how to write my program to have a tighter control on how it runs
so that if the user inputs something incorrectly the program doesn't crash.
"""
from sys import argv

class Product:
    """Abstraction of the drink. Responsible for knowing its price and recipe. Dispenses the drink"""
    def __init__(self, name, price, recipe):
        self.name = name
        self.price = price
        self. recipe = recipe
        
    def getPrice(self):
        """Returns price of product when called"""
        return self.price
    def make(self):
        return self.recipe


class CashBox:
    """Abstraction of a cashbox/change maker on a real machine.
    Responsible for accepting and tracking coins, making change."""
    def __init__(self, credit, totalReceived):
        """Constructer: credit variable for pending credit, totalReceived variable used
            to keep track of total money received after sales"""
        self.credit = credit
        self.totalReceived = totalReceived
        
    def deposit(self, amount):
        """Method used to deposit coins into CahseBox"""
        acceptableValues = [5, 10, 25, 50]#nickles, dimes, quarters, half-dolars
        try:
            amount = int(amount)
        except:
            pass
        if amount not in acceptableValues:
            print("INVALID AMOUNT >>>\nWe only take half-dollars, quarters, dimes, and nickels.\n")
            
        else:
            
            self.credit+= amount
            print(f"Depositing {amount} cents. You have {self.credit} cents credit.\n")
        return self.credit, self.totalReceived
    
    def returnCoins(self):
        """Returns any left over coins"""
        self.change = self.credit
        self.credit = 0
        return self.change
    
    def haveYou(self, amount):
        """This method checks to see if there is enough credit in the CasheBox to cover
            the price of desired product."""
        self.checking = amount
        return self.credit >= self.checking
    
    def deduct(self, amount):
        """This method when called deducts the price of the product from pending credit
            and returns remaining credit."""
        self.deduction = amount
        self.credit -= self.deduction
        self.totalReceived += self.deduction
        return self.credit
    
    def total(self):
        """When called returns total price earned from completed transactions"""
        return self.totalReceived
    
      
class Selector:
    """Abstraction of the internal control mechanism. Knows products & selection, 
        coordinates payment and drink making."""
    def __init__(self, cashBox, products):
        """Constructor receives cashBox object, and list of all the products from CoffeMachine""" 
        self.products = products
        self.cashBox = cashBox
        
    def select(self, choiceIndex):
        """Coordinate the validation and dispensing of a user’s selection"""
        try:
            choiceIndex = int(choiceIndex)
            productPrice = self.products[choiceIndex-1].getPrice()
            if self.cashBox.haveYou(productPrice) == True:
                print("Making", self.products[choiceIndex-1].name + ":")
                for i in self.products[choiceIndex-1].make():
                    print("\tDispensing", i)
                self.cashBox.deduct(productPrice)
                if self.cashBox.credit > 0:
                    print("Returning ", self.cashBox.returnCoins(), "cents.\n")
                else:
                    print()
            else:
                print("Sorry. Not enough money deposited.\n")            
        except:
            print("INVALID SELECTION: Please select a number associated with desired product.\n")
            

class CoffeMachine:
    """Abstraction of the outer machine, holding all the parts.
        Responsible for constructing machine, capturing external input."""
    def __init__(self):
        """Creates all the products, creates the CashBox and creates a products list to
            pass into Selector"""
        #menu
        self.black = Product("black", 35, ("cup", "coffee", "water"))
        self.white = Product("white", 35, ("cup", "coffe", "creamer", "water"))
        self.sweet = Product("sweet", 35, ("cup", "coffe", "sugar", "water"))
        self.white_sweet = Product("white & sweet", 35, ("cup", "coffe", "sugar", "creamer", "water"))
        self.bouillon = Product("bouillon", 25, ("cup", "bouillonPowder", "water"))
        self.products = [self.black, self.white, self.sweet, self.white_sweet, self.bouillon]
        
        self.cashBox = CashBox(0,0)
        self.selector = Selector(self.cashBox, self.products)#Creates the Selector class and passes it the CashBox and list of products
    
    def oneAction(self):
        """ Performs one action, either insert, select, cancel, or quit depending on external input from user.
            This method continues to loop until user enters "quit"."""
        
        print("-" * 40, "\n\tPRODUCT LIST: all 35 cents, except bouillon (25 cents)")
        print("\t1=black, 2=white, 3=sweet, 4=white & sweet, 5=bouillon \n\tSample commands: insert 25, select 1")
        command = input(">>>\tYour command: ")
        action = command.split()
        if len(action) > 1:
            integer = action[1]#integer used for indexing or represents money depending on what class is being used
        
        if "insert" in action:
            try:
                self.cashBox.deposit(int(integer))
            except:
                print("Invalid: No coins were inserted. We only take half-dollars, quarters, dimes, and nickels.\n")
        elif "select" in action:
            try:
                self.selector.select(integer)
            except:
                print("Invalid: Select a number assosicated with desired product from list\n")
            
        elif "cancel" in action:
            if self.cashBox.credit > 0:
                print("Returning", self.cashBox.returnCoins(), "cents.\n")
                
        elif "quit" in action:
            return False
        else:
            print("Invalid command.\n")
        return True
        
    def totalCash(self):
        """Returns total price earned from completed transactions"""
        return self.cashBox.total()


def main():
    """Loops m.oneAction() until user enters quit, then prints total cash received"""
    m = CoffeMachine()
    while m.oneAction():
        pass
    total = m.totalCash()
    print(f"Total cash received: ${total/100:.2f}")

if __name__ == "__main__":
    main()
    
