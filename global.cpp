#include "global.h"
// namespace myglut{

  #define CLUST_COMPARE_MIN 10

  int KNN_MAX;
  int KNN_USE; // 100
  int NDESIRED; // 16000
  int RAND_ITER_MAX; // 8

  GLUT2d* myglut2d;
  GLUT3d* myglut3d;
  GLUT2d* myglut2d_img;

  GLvoid * source;

  clust_knn* myclust_knn;

  vector < SA<float> * > float_buffers;
  SA<int> * i_coord;
  SA<int> * j_coord;

  double avg_surprise;
  int beforelaststate; // save second most recent pick state.
  int number_of_classes, surprising_count;
  int _mouseLeft, _mouseRight, _lastmouseRight, threadcreated, PickThis, lastpick, laststate; // save most recent pick state
  int density_estimate, n_density_estimates, n_skip, select_distance_function;

  FILE * outfile;

  // parallelism stuff
  pthread_mutex_t print_mtx;
  pthread_attr_t pthread_attr; // specify threads joinable
  pthread_mutex_t pthread_next_j_mtx; // work queue
  size_t pthread_next_j; // next job to run
  size_t pthread_start_j, pthread_end_j; // start and end indices for job
  void (*pthread_eval)(size_t); // function pointer to execute in parallel, over range start_j:end_j inclusive

  /*
  void err(char * msg){
    printf("Error: %s\n", msg);
    exit(1);
  }

  void err(string msg) {
    err(msg.c_str());
  }
  */


  void init_mtx(){
    // mutex setup
    pthread_mutex_init(&print_mtx, NULL);
    pthread_mutex_init(&pthread_next_j_mtx, NULL);
  }

  /*
  void cprint(string s){
    pthread_mutex_lock(&print_mtx);
    cout << s << endl;
    pthread_mutex_unlock(&print_mtx);
  }

  */

  void * worker_fun(void * arg){
    size_t k, my_next_j;
    k = (size_t)arg;
    //cprint(str("worker_fun(") + std::to_string(k) + str(")"));

    while(1){
      // try to pick up a job
      mtx_lock(&pthread_next_j_mtx);
      my_next_j = pthread_next_j ++; // index of data this thread should pick up if it can
      mtx_unlock(&pthread_next_j_mtx);

      if(my_next_j >= pthread_end_j){
        //cprint(str("\texit thread ") + to_string(k));
        return(NULL);
      }
      //cprint(str("job ") + to_string(k));
      pthread_eval(my_next_j); // perform action segment
    }
  }

  void parfor(size_t start_j, size_t end_j, void(*eval)(size_t)){
    pthread_eval = eval; // set global function pointer
    pthread_start_j = start_j;
    pthread_end_j = end_j;

    pthread_next_j = start_j; // pthread_next_j_mtx is the lock on this variable
    size_t n_cores = sysconf(_SC_NPROCESSORS_ONLN);
    cout << "Number of threads: " << n_cores << endl;

    // allocate threads, make threads joinable whatever that means
    pthread_attr_init(&pthread_attr);
    pthread_attr_setdetachstate(&pthread_attr, PTHREAD_CREATE_JOINABLE);
    pthread_t * my_pthread = new pthread_t[n_cores];
    size_t j;
    for0(j, n_cores){
      pthread_create(&my_pthread[j], &pthread_attr, worker_fun, (void *)j);
      setThreadAffinity(my_pthread[j], j % n_cores); // Set CPU affinity
    }

    // wait for threads to finish
    for0(j, n_cores){
      pthread_join(my_pthread[j], NULL);
    }

    delete[] my_pthread;
    pthread_attr_destroy(&pthread_attr);
    //cprint(str("return parfor()"));
  }

void setThreadAffinity(pthread_t thread, int cpuIndex) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(cpuIndex, &cpuset);
    pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
}

unsigned int crappy_rand(){
    // linear congruential generator (LCG)
    static unsigned int seed = 0; // Initial seed value
    unsigned int a = 1103515245; // Multiplier
    unsigned int c = 12345; // Increment
    unsigned int m = 2147483648; // Modulus (2^31)
    seed = (a * seed + c) % m; // Update the seed
    return seed;
}
