'''
int sign(int x){
    if(x>=0){
        return 1;
    }
    return -1; 
}

int dist(int t, int r, int x){
    return t - 4*r*r + 4*r - 2*r*x;
}

void dims(int t,int *x,int *z){
    int r = (int)(sqrt(t) + 1) / 2;
    if(r==0) {
        *x=0;
        *z=0;
        return;
    }

    int p = (int)(t - (2*r - 1)*(2*r - 1)) / (2*r);

    
    if (p == 0){
        *x = -r + dist(t, r, p);
        *z = r;
    }
    else if(p == 1){
        *x = r;
        *z = r - dist(t, r, p);
    }
    else if( p == 2){
        *x = r - dist(t, r, p);
        *z = -r;
    }
    else if( p == 3){
        *x = -r;
        *z = -r + dist(t, r, p);
    }

void invDim(int x, int z, int *t){
    if(x==0 && y==0){
        *t=0;
        return;
    }

    int *t = abs(4*x*x - 2*x + sign(x)*x - sign(x)*z) ;
    int *tmp_x,*tmp_z
    dims(t,x,z);
    if( x == *tmp_x && z == *tmp_z):
        return ;
    else{
        *t = sign(z)*x + 4*z*z - 2*z - sign(z)*z;
    }
}
}

'''



sign = lambda x: 0 if not x else int(x/abs(x))

def dist(t, r, x):
    return int(t - 4*r**2 + 4*r - 2*r*x)

def find_xy(t):
    r = int((t**(0.5) + 1) / 2)
    if r == 0:
        return 0, 0
    
    p = int((t - (2*r - 1)**2) / (2*r))

    if p == 0:
        x = -r + dist(t, r, p)
        y = r
    elif p == 1:
        x = r
        y = r - dist(t, r, p)
    elif p == 2:
        x = r - dist(t, r, p)
        y = -r
    elif p == 3:
        x = -r
        y = -r + dist(t, r, p)

    return x, y

for i in range(10):
    for j in range(10):
    

        x = i - 10/2
        y = j - 10/2
        print(f"Give the x: {x}")
        print(f"Give the y: {y}")

        if x == 0 and y == 0:
            print(0)
            continue


        t = 4*x**2 - 2*x + sign(x)*x - sign(x)*y
        x_new, y_new = find_xy(abs(t))
        if x == x_new and y == y_new:
            print(t)
        else:
            t = sign(y)*x + 4*y**2 - 2*y - sign(y)*y
            print(t)
        
        print()
