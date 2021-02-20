# KR_Banking
 

 # Developer Docs: 

 #1 ---Serverside---
 int playersBankAmount = GetBankingServerManager().GetPlayersBankAmount(identity);
 This will return you The CurrencyAmount what a player has on his bankaccount.

 ---Clientside---

/* 
    @GetBankCredits()
        returns Currency on Bank Account from Player.
*/
EXAMPLE:

 int currencyOnPlayersBankAmount = GetBankingClientManager().GetBankCredits();





 /* 
    @GetPlayerCurrencyAmount()
        returns the Amount of Currency what a player has in Inventory.
 */
 EXAMPLE: 

 int currencyOnPlayerAmount = GetBankingClientManager().GetPlayerCurrencyAmount();

