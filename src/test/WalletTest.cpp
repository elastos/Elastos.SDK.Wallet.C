
#include "Transaction.h"
#include "BlockChainNode.h"
#include "WalletError.h"
#include "Elastos.Wallet.Utility.h"
#include "common/Utils.h"
#include <vector>
#include "Identity.h"
#include "IdentityManager.h"
#include "Did.h"

using namespace elastos;

void TestSingleWallet();

int main(int argc, char** argv)
{
    TestSingleWallet();
    return 0;
}

char* readMnemonicFile(const char* path)
{
    FILE* file = fopen(path, "r");
    if (!file) {
        return nullptr;
    }
    char* buf = (char*)malloc(1024 * 10);
    if (!buf) {
        fclose(file);
        return nullptr;
    }
    int count = 0;
    char c;

    while ( (c = fgetc(file)) != EOF) {
        buf[count++] = c;
    }

    buf[count] = '\0';
    fclose(file);

    return buf;
}

void TestSingleWallet()
{
    const char* mnemonic = "血 坝 告 售 代 讨 转 枝 欧 旦 诚 抱";
    const char* path = "/Users/nathansfile/Elastos.ORG.Wallet.Lib.C/src/Data/mnemonic_chinese.txt";
    char* words = readMnemonicFile(path);
    if (!words) {
        printf("read file failed\n");
        return;
    }

    std::string seed = IdentityManager::GetSeed(mnemonic, "chinese", words, "");
    printf("seed:%s\n", seed.c_str());

    std::shared_ptr<Identity> identity;
    int index = IdentityManager::CreateIdentity("/Users/nathansfile/Elastos.SDK.Wallet.C", &identity);
    printf("identity index:%d\n", index);


    std::shared_ptr<HDWallet> hdWallet;
    index = identity->CreateSingleAddressWallet(seed, &hdWallet);
    printf("hd wallet index:%d\n", index);

    std::string publicKey = hdWallet->GetPublicKey(0, 0);
    std::string hdSingleAddress = hdWallet->GetAddress(0, 0);
    printf("hd address:%s\n", hdSingleAddress.c_str());

    long balance = hdWallet->GetBalance(hdSingleAddress);
    printf("balance: %ld\n", balance);

    // Transaction tx("EdyqqiJcdkTDtfkvxVbTuNXGMdB3FEcpXA", 100000000L, "");
    // std::vector<Transaction> transactions;
    // transactions.push_back(tx);
    // std::string txid;
    // int ret = hdWallet->SendTransaction(transactions, seed, txid);
    // if (ret != E_WALLET_C_OK) {
    //     printf("send transaction failed: %d\n", ret);
    //     return;
    // }

    // printf("txid: %s\n", txid.c_str());

    // balance = wallet->GetBalance();
    // printf("balance: %ld\n", balance);

    Did didObj(publicKey);
    std::string did = didObj.GetId();
    printf("did: %s\n", did.c_str());

    std::string str("{\"key\": \"name\", \"value\":\"alice\"}");
    std::string info = didObj.SetInfo(seed, 0, str);
    printf("signed info: %s\n", info.c_str());

}
