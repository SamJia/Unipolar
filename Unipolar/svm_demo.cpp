#include "common.h"
#include "smo.h"
#include "drand48.h"

typedef vector<pair<int, float> > sparse_vector;
int main(int argc, char *argv[]){

    extern char *optarg;
    extern int optind;
    int c;
    int errflg = 0;

    svm_option *opt = new svm_option();

    while ((c = getopt(argc, argv, "c:t:e:s:p:q:m:o:r:l")) != EOF) {
        switch(c) {
            case 'c': opt->C = atof(optarg); break;
            case 't': opt->tol = atof(optarg); break;
            case 'e': opt->eps = atof(optarg); break;
            case 's': opt->sigma = atof(optarg); break;
            case 'p': opt->train_path = optarg; break;
            case 'q': opt->test_path = optarg; break;
            case 'm': opt->model_path = optarg; break;
            case 'o': opt->output_path = optarg; break;
            case 'l': opt->is_linear_kernel = true; break;
            case 'r': srand48(atoi(optarg)); break;
            case '?': errflg++;
        }
    }

    if (errflg || optind < argc){
        cerr << "usage: "  << argv[0] << " -c C -t tolerance -e epsilon -s sigma -p train_path -q test_path \
            -m model_path -o output_path -r random_seed -l (is_linear_kernel)" << endl;
        exit(1);
    }

    SMO *s = new SMO(opt);
    // s->train();
    s->readyPredict();
    sparse_vector x;
x.push_back(pair<int,float>(1,-0.5));
x.push_back(pair<int,float>(2,-1));
x.push_back(pair<int,float>(3,-0.333333));
x.push_back(pair<int,float>(4,-0.396226));
x.push_back(pair<int,float>(5,-0.178082));
x.push_back(pair<int,float>(6,-1));
x.push_back(pair<int,float>(7,-1));
x.push_back(pair<int,float>(8,0.40458));
x.push_back(pair<int,float>(9,-1));
x.push_back(pair<int,float>(10,-1));
x.push_back(pair<int,float>(11,-1));
x.push_back(pair<int,float>(12,-1));
x.push_back(pair<int,float>(13,-1));
    cout<<s->predict(x)<<endl;
    cout<<s->predict(x)<<endl;
    cout<<s->predict(x)<<endl;
    cout<<s->predict(x)<<endl;
    cout<<s->predict(x)<<endl;
}

