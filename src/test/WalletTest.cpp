
#include "Transaction.h"
#include "BlockChainNode.h"
#include "WalletError.h"
#include "Elastos.Wallet.Utility.h"
#include "common/Utils.h"
#include <vector>
#include "Identity.h"
#include "IdentityManager.h"
#include "Did.h"
#include "DidManager.h"

using namespace elastos;

void TestSingleWallet();
void TestHDWallet();

int main(int argc, char** argv)
{
    // TestSingleWallet();
    TestHDWallet();
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
    int ret;
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


    std::unique_ptr<BlockChainNode> node1 = std::make_unique<BlockChainNode>(TEST_NET_WALLET_SERVICE_URL);
    std::shared_ptr<HDWallet> hdWallet;
    index = identity->CreateSingleAddressWallet(seed, node1, &hdWallet);
    printf("hd wallet index:%d\n", index);

    std::string publicKey = hdWallet->GetPublicKey(0, 0);
    std::string hdSingleAddress = hdWallet->GetAddress(0, 0);
    printf("hd address:%s\n", hdSingleAddress.c_str());

    long balance = hdWallet->GetBalance(hdSingleAddress);
    printf("balance: %ld\n", balance);

    // hdWallet->SyncHistory();

    // std::unique_ptr<BlockChainNode> node = std::make_unique<BlockChainNode>(TEST_NET_DID_SERVICE_URL);
    // std::shared_ptr<HDWallet> idChainWallet;
    // index = identity->CreateSingleAddressWallet(seed, node, &idChainWallet);

    // balance = idChainWallet->GetBalance(hdSingleAddress);
    // printf("id chain balance: %ld\n", balance);

    // idChainWallet->SyncHistory();


    std::vector<Transaction> transactions;
    Transaction tx1("EdVgb5RTdmwKf79pEUdVNnFprWyvmr1hPc", 100000000L);
    transactions.push_back(tx1);
    Transaction tx2("ETajy5FmdzhngRUJmwKJFJBMv8EM8BhXnH", 100000000L);
    transactions.push_back(tx2);
    Transaction tx3("ENvWkDwYdt1m5F2Pi7WEVDXk89T8qne6jj", 50000000L);
    transactions.push_back(tx3);
    Transaction tx4("Ed2656EoB37fr2DSnqbLvp8G5cxDtZKYf4", 20000000L);
    transactions.push_back(tx4);
    Transaction tx5("EUFDLS15BFZHMGkRHTwM6Sz4gYuoejXrHZ", 10000000L);
    transactions.push_back(tx5);

    std::string txid;
    ret = hdWallet->SendTransaction(transactions, "", seed, txid);
    if (ret != E_WALLET_C_OK) {
        printf("send transaction failed: %d\n", ret);
        return;
    }

    printf("txid: %s\n", txid.c_str());


    std::string histories;
    int count = hdWallet->GetHistoryCount(hdSingleAddress);
    hdWallet->GetHistory(hdSingleAddress, 5, 0, false, histories);
    printf("history: %s\n", histories.c_str());

    // balance = wallet->GetBalance();
    // printf("balance: %ld\n", balance);

    std::shared_ptr<DidManager> manager;
    ret = identity->CreateDidManager(seed, &manager);
    if (ret != 0) {
        printf("create Did manager failed\n");
        return;
    }

    std::shared_ptr<Did> didObj;
    index = manager->CreateDid(0, &didObj);
    std::string did = didObj->GetId();
    printf("did: %s\n", did.c_str());

    // std::string str("[{\"Key\": \"name\", \"Value\":\"alice\"}]");
    // std::string info = didObj->SignInfo(seed, str);
    // printf("signed info: %s\n", info.c_str());

    // std::string txid = didObj->SetInfo(seed, str, idChainWallet);
    // printf("did set info txid: %s\n", txid.c_str());

    didObj->SyncInfo();

    std::string info = didObj->GetInfo("name");
    printf("the did info: %s\n", info.c_str());

}

void TestHDWallet()
{
    int ret;
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

    std::unique_ptr<BlockChainNode> node1 = std::make_unique<BlockChainNode>(TEST_NET_WALLET_SERVICE_URL);
    std::shared_ptr<HDWallet> hdWallet;
    index = identity->CreateWallet(seed, COIN_TYPE_ELA, node1, &hdWallet);
    printf("hd wallet index:%d\n", index);

    // hdWallet->SyncHistory();
    hdWallet->Recover();

    int balance = hdWallet->GetBalance();
    printf("balance: %d\n", balance);

    // std::vector<Transaction> transactions;
    // Transaction tx1("ENvWkDwYdt1m5F2Pi7WEVDXk89T8qne6jj", 100000000L);
    // transactions.push_back(tx1);

    // std::string txid;
    // ret = hdWallet->SendTransaction(transactions, "", seed, txid);
    // if (ret != E_WALLET_C_OK) {
    //     printf("send transaction failed: %d\n", ret);
    //     return;
    // }
    // printf("txid: %s\n", txid.c_str());

    std::vector<std::string> usedAddrs = hdWallet->GetUsedAddresses();
    printf("used address:\n");
    for (std::string addr : usedAddrs) {
        printf("%s\n", addr.c_str());
    }
}
