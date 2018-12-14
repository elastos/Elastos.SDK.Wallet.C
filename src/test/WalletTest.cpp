
#include "SingleWallet.h"
#include "Transaction.h"
#include "BlockChainNode.h"
#include "WalletError.h"
#include "Elastos.Wallet.Utility.h"
#include "common/Utils.h"
#include <vector>

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
    std::shared_ptr<BlockChainNode> node(new BlockChainNode("https://hw-ela-api-test.elastos.org"));

    const char* mnemonic = "血 坝 告 售 代 讨 转 枝 欧 旦 诚 抱";
    const char* path = "/Users/nathansfile/Elastos.ORG.Wallet.Lib.C/src/Data/mnemonic_chinese.txt";
    char* words = readMnemonicFile(path);
    if (!words) {
        printf("read file failed\n");
        return;
    }

    void* seed;
    int seedLen = getSeedFromMnemonic(&seed, mnemonic, "chinese", words, "");

    std::string seedstr = Utils::Hex2Str((const uint8_t *)seed, seedLen);
    printf("seed: %s\n", seedstr.c_str());
    free(seed);

    SingleWallet wallet(seedstr, node);

    long balance = wallet.GetBalance();
    printf("balance: %ld\n", balance);

    Transaction tx("EdyqqiJcdkTDtfkvxVbTuNXGMdB3FEcpXA", 100000000L, "");
    std::vector<Transaction> transactions;
    transactions.push_back(tx);
    std::string txid;
    int ret = wallet.SendTransaction(transactions, seedstr, txid);
    if (ret != E_WALLET_C_OK) {
        printf("send transaction failed: %d\n", ret);
        return;
    }

    printf("txid: %s\n", txid.c_str());

    balance = wallet.GetBalance();
    printf("balance: %ld\n", balance);
}
