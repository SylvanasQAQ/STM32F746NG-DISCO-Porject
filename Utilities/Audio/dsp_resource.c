#include "main.h"

float32_t hamming256[] = {8.0000000e-021,
                          8.0139632e-02,
                          8.0558444e-02,
                          8.1256180e-02,
                          8.2232418e-02,
                          8.3486566e-02,
                          8.5017860e-02,
                          8.6825373e-02,
                          8.8908006e-02,
                          9.1264495e-02,
                          9.3893409e-02,
                          9.6793154e-02,
                          9.9961967e-02,
                          1.0339793e-01,
                          1.0709894e-01,
                          1.1106277e-01,
                          1.1528701e-01,
                          1.1976909e-01,
                          1.2450629e-01,
                          1.2949573e-01,
                          1.3473439e-01,
                          1.4021908e-01,
                          1.4594648e-01,
                          1.5191311e-01,
                          1.5811535e-01,
                          1.6454942e-01,
                          1.7121143e-01,
                          1.7809733e-01,
                          1.8520294e-01,
                          1.9252394e-01,
                          2.0005590e-01,
                          2.0779423e-01,
                          2.1573425e-01,
                          2.2387112e-01,
                          2.3219992e-01,
                          2.4071558e-01,
                          2.4941294e-01,
                          2.5828671e-01,
                          2.6733150e-01,
                          2.7654184e-01,
                          2.8591211e-01,
                          2.9543665e-01,
                          3.0510965e-01,
                          3.1492526e-01,
                          3.2487751e-01,
                          3.3496036e-01,
                          3.4516768e-01,
                          3.5549329e-01,
                          3.6593092e-01,
                          3.7647422e-01,
                          3.8711679e-01,
                          3.9785218e-01,
                          4.0867387e-01,
                          4.1957529e-01,
                          4.3054981e-01,
                          4.4159078e-01,
                          4.5269150e-01,
                          4.6384522e-01,
                          4.7504517e-01,
                          4.8628456e-01,
                          4.9755655e-01,
                          5.0885432e-01,
                          5.2017099e-01,
                          5.3149970e-01,
                          5.4283358e-01,
                          5.5416573e-01,
                          5.6548928e-01,
                          5.7679736e-01,
                          5.8808309e-01,
                          5.9933964e-01,
                          6.1056016e-01,
                          6.2173785e-01,
                          6.3286591e-01,
                          6.4393759e-01,
                          6.5494618e-01,
                          6.6588498e-01,
                          6.7674735e-01,
                          6.8752671e-01,
                          6.9821650e-01,
                          7.0881024e-01,
                          7.1930150e-01,
                          7.2968391e-01,
                          7.3995115e-01,
                          7.5009701e-01,
                          7.6011532e-01,
                          7.7000000e-01,
                          7.7974505e-01,
                          7.8934454e-01,
                          7.9879267e-01,
                          8.0808368e-01,
                          8.1721193e-01,
                          8.2617190e-01,
                          8.3495812e-01,
                          8.4356529e-01,
                          8.5198815e-01,
                          8.6022162e-01,
                          8.6826067e-01,
                          8.7610044e-01,
                          8.8373617e-01,
                          8.9116321e-01,
                          8.9837706e-01,
                          9.0537335e-01,
                          9.1214782e-01,
                          9.1869636e-01,
                          9.2501499e-01,
                          9.3109988e-01,
                          9.3694734e-01,
                          9.4255381e-01,
                          9.4791590e-01,
                          9.5303033e-01,
                          9.5789403e-01,
                          9.6250401e-01,
                          9.6685750e-01,
                          9.7095185e-01,
                          9.7478456e-01,
                          9.7835332e-01,
                          9.8165596e-01,
                          9.8469047e-01,
                          9.8745501e-01,
                          9.8994790e-01,
                          9.9216763e-01,
                          9.9411285e-01,
                          9.9578238e-01,
                          9.9717520e-01,
                          9.9829048e-01,
                          9.9912753e-01,
                          9.9968585e-01,
                          9.9996509e-01,
                          9.9996509e-01,
                          9.9968585e-01,
                          9.9912753e-01,
                          9.9829048e-01,
                          9.9717520e-01,
                          9.9578238e-01,
                          9.9411285e-01,
                          9.9216763e-01,
                          9.8994790e-01,
                          9.8745501e-01,
                          9.8469047e-01,
                          9.8165596e-01,
                          9.7835332e-01,
                          9.7478456e-01,
                          9.7095185e-01,
                          9.6685750e-01,
                          9.6250401e-01,
                          9.5789403e-01,
                          9.5303033e-01,
                          9.4791590e-01,
                          9.4255381e-01,
                          9.3694734e-01,
                          9.3109988e-01,
                          9.2501499e-01,
                          9.1869636e-01,
                          9.1214782e-01,
                          9.0537335e-01,
                          8.9837706e-01,
                          8.9116321e-01,
                          8.8373617e-01,
                          8.7610044e-01,
                          8.6826067e-01,
                          8.6022162e-01,
                          8.5198815e-01,
                          8.4356529e-01,
                          8.3495812e-01,
                          8.2617190e-01,
                          8.1721193e-01,
                          8.0808368e-01,
                          7.9879267e-01,
                          7.8934454e-01,
                          7.7974505e-01,
                          7.7000000e-01,
                          7.6011532e-01,
                          7.5009701e-01,
                          7.3995115e-01,
                          7.2968391e-01,
                          7.1930150e-01,
                          7.0881024e-01,
                          6.9821650e-01,
                          6.8752671e-01,
                          6.7674735e-01,
                          6.6588498e-01,
                          6.5494618e-01,
                          6.4393759e-01,
                          6.3286591e-01,
                          6.2173785e-01,
                          6.1056016e-01,
                          5.9933964e-01,
                          5.8808309e-01,
                          5.7679736e-01,
                          5.6548928e-01,
                          5.5416573e-01,
                          5.4283358e-01,
                          5.3149970e-01,
                          5.2017099e-01,
                          5.0885432e-01,
                          4.9755655e-01,
                          4.8628456e-01,
                          4.7504517e-01,
                          4.6384522e-01,
                          4.5269150e-01,
                          4.4159078e-01,
                          4.3054981e-01,
                          4.1957529e-01,
                          4.0867387e-01,
                          3.9785218e-01,
                          3.8711679e-01,
                          3.7647422e-01,
                          3.6593092e-01,
                          3.5549329e-01,
                          3.4516768e-01,
                          3.3496036e-01,
                          3.2487751e-01,
                          3.1492526e-01,
                          3.0510965e-01,
                          2.9543665e-01,
                          2.8591211e-01,
                          2.7654184e-01,
                          2.6733150e-01,
                          2.5828671e-01,
                          2.4941294e-01,
                          2.4071558e-01,
                          2.3219992e-01,
                          2.2387112e-01,
                          2.1573425e-01,
                          2.0779423e-01,
                          2.0005590e-01,
                          1.9252394e-01,
                          1.8520294e-01,
                          1.7809733e-01,
                          1.7121143e-01,
                          1.6454942e-01,
                          1.5811535e-01,
                          1.5191311e-01,
                          1.4594648e-01,
                          1.4021908e-01,
                          1.3473439e-01,
                          1.2949573e-01,
                          1.2450629e-01,
                          1.1976909e-01,
                          1.1528701e-01,
                          1.1106277e-01,
                          1.0709894e-01,
                          1.0339793e-01,
                          9.9961967e-02,
                          9.6793154e-02,
                          9.3893409e-02,
                          9.1264495e-02,
                          8.8908006e-02,
                          8.6825373e-02,
                          8.5017860e-02,
                          8.3486566e-02,
                          8.2232418e-02,
                          8.1256180e-02,
                          8.0558444e-02,
                          8.0139632e-02,
                          8.0000000e-02};

float32_t hamming128[] = {
    8.0000000e-02,
    8.0562849e-02,
    8.2250017e-02,
    8.5057376e-02,
    8.8978056e-02,
    9.4002462e-02,
    1.0011830e-01,
    1.0731060e-01,
    1.1556177e-01,
    1.2485160e-01,
    1.3515738e-01,
    1.4645387e-01,
    1.5871343e-01,
    1.7190607e-01,
    1.8599949e-01,
    2.0095922e-01,
    2.1674863e-01,
    2.3332909e-01,
    2.5066003e-01,
    2.6869903e-01,
    2.8740195e-01,
    3.0672302e-01,
    3.2661496e-01,
    3.4702909e-01,
    3.6791545e-01,
    3.8922293e-01,
    4.1089938e-01,
    4.3289177e-01,
    4.5514627e-01,
    4.7760842e-01,
    5.0022325e-01,
    5.2293542e-01,
    5.4568935e-01,
    5.6842936e-01,
    5.9109980e-01,
    6.1364519e-01,
    6.3601036e-01,
    6.5814057e-01,
    6.7998167e-01,
    7.0148022e-01,
    7.2258359e-01,
    7.4324016e-01,
    7.6339936e-01,
    7.8301186e-01,
    8.0202967e-01,
    8.2040626e-01,
    8.3809664e-01,
    8.5505753e-01,
    8.7124742e-01,
    8.8662669e-01,
    9.0115771e-01,
    9.1480492e-01,
    9.2753491e-01,
    9.3931655e-01,
    9.5012099e-01,
    9.5992179e-01,
    9.6869497e-01,
    9.7641907e-01,
    9.8307517e-01,
    9.8864700e-01,
    9.9312091e-01,
    9.9648596e-01,
    9.9873391e-01,
    9.9985927e-01,
    9.9985927e-01,
    9.9873391e-01,
    9.9648596e-01,
    9.9312091e-01,
    9.8864700e-01,
    9.8307517e-01,
    9.7641907e-01,
    9.6869497e-01,
    9.5992179e-01,
    9.5012099e-01,
    9.3931655e-01,
    9.2753491e-01,
    9.1480492e-01,
    9.0115771e-01,
    8.8662669e-01,
    8.7124742e-01,
    8.5505753e-01,
    8.3809664e-01,
    8.2040626e-01,
    8.0202967e-01,
    7.8301186e-01,
    7.6339936e-01,
    7.4324016e-01,
    7.2258359e-01,
    7.0148022e-01,
    6.7998167e-01,
    6.5814057e-01,
    6.3601036e-01,
    6.1364519e-01,
    5.9109980e-01,
    5.6842936e-01,
    5.4568935e-01,
    5.2293542e-01,
    5.0022325e-01,
    4.7760842e-01,
    4.5514627e-01,
    4.3289177e-01,
    4.1089938e-01,
    3.8922293e-01,
    3.6791545e-01,
    3.4702909e-01,
    3.2661496e-01,
    3.0672302e-01,
    2.8740195e-01,
    2.6869903e-01,
    2.5066003e-01,
    2.3332909e-01,
    2.1674863e-01,
    2.0095922e-01,
    1.8599949e-01,
    1.7190607e-01,
    1.5871343e-01,
    1.4645387e-01,
    1.3515738e-01,
    1.2485160e-01,
    1.1556177e-01,
    1.0731060e-01,
    1.0011830e-01,
    9.4002462e-02,
    8.8978056e-02,
    8.5057376e-02,
    8.2250017e-02,
    8.0562849e-02,
    8.0000000e-02,

};

float32_t hamming64[] = {
    8.0000000e-02,
    8.2285843e-02,
    8.9120656e-02,
    1.0043651e-01,
    1.1612094e-01,
    1.3601808e-01,
    1.5993016e-01,
    1.8761956e-01,
    2.1881106e-01,
    2.5319469e-01,
    2.9042872e-01,
    3.3014310e-01,
    3.7194313e-01,
    4.1541338e-01,
    4.6012184e-01,
    5.0562416e-01,
    5.5146812e-01,
    5.9719810e-01,
    6.4235963e-01,
    6.8650386e-01,
    7.2919207e-01,
    7.7000000e-01,
    8.0852209e-01,
    8.4437549e-01,
    8.7720386e-01,
    9.0668095e-01,
    9.3251381e-01,
    9.5444568e-01,
    9.7225861e-01,
    9.8577555e-01,
    9.9486218e-01,
    9.9942818e-01,
    9.9942818e-01,
    9.9486218e-01,
    9.8577555e-01,
    9.7225861e-01,
    9.5444568e-01,
    9.3251381e-01,
    9.0668095e-01,
    8.7720386e-01,
    8.4437549e-01,
    8.0852209e-01,
    7.7000000e-01,
    7.2919207e-01,
    6.8650386e-01,
    6.4235963e-01,
    5.9719810e-01,
    5.5146812e-01,
    5.0562416e-01,
    4.6012184e-01,
    4.1541338e-01,
    3.7194313e-01,
    3.3014310e-01,
    2.9042872e-01,
    2.5319469e-01,
    2.1881106e-01,
    1.8761956e-01,
    1.5993016e-01,
    1.3601808e-01,
    1.1612094e-01,
    1.0043651e-01,
    8.9120656e-02,
    8.2285843e-02,
    8.0000000e-02,

};

float32_t fir_filter[] = {
    -0.0006885469,
    -0.002483899,
    -0.004760056,
    -0.005211798,
    -0.001431414,
    0.006083973,
    0.01261655,
    0.01240106,
    0.004576596,
    -0.004247917,
    -0.005270929,
    0.0031077,
    0.01189628,
    0.009927871,
    -0.003086976,
    -0.01374793,
    -0.008414635,
    0.00980101,
    0.02083169,
    0.008654698,
    -0.01745665,
    -0.02791545,
    -0.004622317,
    0.03274375,
    0.03981258,
    -0.004855375,
    -0.06435206,
    -0.06408174,
    0.04043479,
    0.2101013,
    0.3398463,
    0.3398463,
    0.2101013,
    0.04043479,
    -0.06408174,
    -0.06435206,
    -0.004855375,
    0.03981258,
    0.03274375,
    -0.004622317,
    -0.02791545,
    -0.01745665,
    0.008654698,
    0.02083169,
    0.00980101,
    -0.008414635,
    -0.01374793,
    -0.003086976,
    0.009927871,
    0.01189628,
    0.0031077,
    -0.005270929,
    -0.004247917,
    0.004576596,
    0.01240106,
    0.01261655,
    0.006083973,
    -0.001431414,
    -0.005211798,
    -0.004760056,
    -0.002483899,
    -0.0006885469};