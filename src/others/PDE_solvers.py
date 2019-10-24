#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Oct 24 13:17:52 2019

@author: lbrahimi
"""

import numpy as np 
import matplotlib.pyplot as plt 

# TriDiagonal matrix inversion function
#def InverseTrigonalMatrix(T) : 
#    n = len(T)
#    
#    a = np.zeros(n)
#    b = np.zeros(n)
#    c = np.zeros(n)
#    for i in range(n) : 
#        b[i] = T[i][i]
#        if (i < n-1) : c[i] = T[i][i+1]
#        if (i > 0) : a[i] = T[i][i-1]
#    
#    theta_mm  = 0
#    theta_m   = 1
#    theta = np.zeros(n)
#    theta[0] = b[0]*theta_m #- a[0]*c[0]*theta_mm
#    theta[1] = b[1]*theta[0] - a[1]*c[0]*theta_m
#    for i in range(2, n, 1) : 
#        theta[i] = b[i]*theta[i-1] - a[i]*c[i-1]*theta[i-2]
#    
#    phi_pp = 0
#    phi_p  = 1
#    phi = np.zeros(n+2)
#    phi[n] = phi_p
#    phi[n+1] = phi_pp
#    phi[n-1] = b[n-1]*phi_p #- c[n-1]*a[n]*phi_pp
#    phi[n-2] = b[n-2]*phi[n-1] - c[n-2]*a[n-1]*phi_p
#    for i in range(n-3, -1, -1) : 
#        phi[i] = b[i]*phi[i+1] - c[i]*a[i+1]*phi[i+2]
#        
#    
#    Tinv = np.empty((n, n))
#    for i in range(n) : 
#        for j in range(n) : 
#    #        print (i,j)
#            if (i < j) : 
#                p = 1.
#                for ei in range(i, j, 1) : 
#                    p = p*c[ei]
#                if (i-1 == -1) : 
#                    loc_theta = theta_m
#                if (i-1 > -1) : 
#                    loc_theta = theta[i-1]
#                
#                Tij = (-1)**(i+j)*p*loc_theta*phi[j+1]/theta[n-1]
#            if (i == j) : 
#                if (i-1 == -1) : 
#                    loc_theta = theta_m
#                if (i-1 > -1) : 
#                    loc_theta = theta[i-1]
#                Tij = loc_theta*phi[j+1]/theta[n-1]
#            if (i > j) : 
#                p = 1.
#                for ei in range(j+1, i+1, 1) : 
#                    p = p*a[ei]
#                if (j-1 == -1) : 
#                    loc_theta = theta_m
#                if (j-1 > -1) : 
#                    loc_theta = theta[j-1]
#                Tij = (-1)**(i+j)*p*loc_theta*phi[i+1]/theta[n-1]
#            Tinv[i, j] = Tij
#    
#    return Tinv 
#
#def ProductMatrix(A, B) : 
#    A_l = len(A)
#    A_c = len(A[0])
#    
#    B_l = len(B)
#    B_c = len(B[0])
#    
#    C_l = A_l
#    C_c = B_c
#    
#    CC = np.zeros((C_l, C_c))
#    for i in range(C_l) : 
#        for j in range(C_c) : 
#            s = 0.
#            for k in range(A_c) : 
#                s += A[i][k]*B[k][j]
#            CC[i][j]=  s 
#
#    return CC

#def ProductMatrix(A, B) : 
#    A_l = len(A)
#    A_c = len(A[0])
#    
#    B_l = len(B)
#    B_c = len(B[0])
#    
#    C_l = A_l
#    C_c = B_c
#    
#    C = np.zeros((C_l, C_c))
#    for i in range(C_l) : 
#        for j in range(C_c) : 
#            s = 0.
#            for k in range(A_c) : 
#                s += A[i][k]*B[k][j]
#            C[i][j]=  s 
#
#    return C

# Thomas Algorithm (Trigonal matrix inversion O(N))
def TDMA(a,b,c,d):
    n = len(d)
    w= np.zeros(n-1,float)
    g= np.zeros(n, float)
    p = np.zeros(n,float)

    w[0] = c[0]/b[0]
    g[0] = d[0]/b[0]

    for i in range(1,n-1):
        w[i] = c[i]/(b[i] - a[i-1]*w[i-1])
    for i in range(1,n):
        g[i] = (d[i] - a[i-1]*g[i-1])/(b[i] - a[i-1]*w[i-1])
    p[n-1] = g[n-1]
    for i in range(n-1,0,-1):
        p[i-1] = g[i-1] - w[i-1]*p[i]
    return p

# Park & Petrosian (1995)

pc = 3.086e18 #[cm]
# We try to solve the equation (1)
# where : 
def A(x) : # phase factor
    return 1. 

def B(x) : # Advection term
    return -1e6 #[cm/s]
#    if (x > 20.*pc) : 
#        return -1e6 #[cm/s]
#    if (x <= 20.*pc) : 
#        return -1.

def C(x) : # Diffusion term
#    return 1e22
    return 1e28*(1 - 0.1*np.exp(-(600.*pc-x)**2/(50*pc)**2)) #[cm^2/s]
#    if (x > 510*pc+20.*pc and x < 510*pc+30.*pc) : 
#        return 1e28
#    else : 
#        return 1e29

    
    

def Q(x) : # Source term
    return 0.

def T(x) : # Escape term
    return np.inf






# Space grid 
M = 8192
Xmin = 0. 
Xmax = 2000.*pc
X = np.linspace(Xmin, Xmax, M+1)

# Time grid
#N = 100
#tau_n = 1.



# Initial condition 
u = np.zeros(M+1)
for ii in range(1, M+1-1) : 
    if (X[ii] >= 480.*pc and X[ii] < 520.*pc) : 
        u[ii] = 1e-11 #[erg/cm^3]
# Boundary conditions over u(x, t)
u0 = u[1] 
uM = u[M-1] 

u[0] = u0
u[M] = uM


#F = np.zeros(M+1)
#dudx = np.zeros(M+1)
#for ii in range(1, M+1-1) : 
#    dudx[ii] = (u[ii+1] - u[ii-1])/(X[ii+1] - X[ii-1])
#    F[ii] = C(X[ii])*dudx[ii] + B(X[ii])*u[ii]
#F[0] = F0
#F[M] = FM






u_ini = u

# Simulation 

yr  = 365.25*86400
kyr = 1e3*yr


def CC70Solver(dt, Tmax, u) :
    t = 0.
    while (t < Tmax) : 
        
        print ("t = ",t/kyr,"/",Tmax/kyr)
        
        
#        TT = np.zeros((M+1, M+1))
        R = np.zeros((1, M+1))
        
        a = np.zeros(M+1)
        b = np.zeros(M+1)
        c = np.zeros(M+1)
        
    
        
        for xm in range(1, M) : 
            
            dx_m    = (X[xm+1] - X[xm-1])/2.
            X_m = 0.5*(X[xm+1] + X[xm-1])
            A_m = A(X_m)
            B_m = B(X_m)
            C_m = C(X_m)
            Q_m = Q(X_m)
            T_m = T(X_m)
            w_m = B_m/C_m*dx_m
            d_m = 1./w_m - 1/(np.exp(w_m) - 1)
            W_m = w_m/2.*1./np.sinh(w_m/2.)
            Wp_m = W_m*np.exp(w_m/2.)
            Wm_m = W_m*np.exp(-w_m/2.)
            
    
            dx_m_mh = X[xm] - X[xm-1]
            X_m_mh = 0.5*(X[xm] + X[xm-1])
            A_m_mh = A(X_m_mh)
            B_m_mh = B(X_m_mh)
            C_m_mh = C(X_m_mh)
            Q_m_mh = Q(X_m_mh)
            w_m_mh = B_m_mh/C_m_mh*dx_m_mh
            d_m_mh = 1./w_m_mh - 1/(np.exp(w_m_mh) - 1)
            W_m_mh = w_m_mh/2.*1./np.sinh(w_m_mh/2.)
            Wp_m_mh = W_m_mh*np.exp(w_m_mh/2.)
            Wm_m_mh = W_m_mh*np.exp(-w_m_mh/2.)
        
        
    
            dx_m_ph = X[xm+1] - X[xm]
            X_m_ph = 0.5*(X[xm+1] + X[xm])
            A_m_ph = A(X_m_ph)
            B_m_ph = B(X_m_ph)
            C_m_ph = C(X_m_ph)
            Q_m_ph = Q(X_m_ph)
            w_m_ph = B_m_ph/C_m_ph*dx_m_ph
            d_m_ph = 1./w_m_ph - 1/(np.exp(w_m_ph) - 1)
            W_m_ph = w_m_ph/2.*1./np.sinh(w_m_ph/2.)
            Wp_m_ph = W_m_ph*np.exp(w_m_ph/2.)
            Wm_m_ph = W_m_ph*np.exp(-w_m_ph/2.)
            
            
            a_m = dt/(A_m*dx_m)*C_m_mh/dx_m_mh*Wm_m_mh
            c_m = dt/(A_m*dx_m)*C_m_ph/dx_m_ph*Wp_m_ph
            b_m = 1 + dt/(A_m*dx_m)*(C_m_mh/dx_m_mh*Wp_m_mh + C_m_ph/dx_m_ph*Wm_m_ph) + dt/T_m
            
            a[xm] = -a_m
            b[xm] = b_m
            c[xm] = -c_m
    
    #        TT[xm][xm]   =  b_m
    #        TT[xm+1][xm] = -a_m
    #        TT[xm][xm+1] = -c_m
            r_m = dt*Q_m + u[xm]
            R[0][xm] = r_m
            
        # Cas xm = 0
        dx_m = X[1] - X[0]
        X_m = X[0]
        A_m = A(X_m)
        B_m = B(X_m)
        C_m = C(X_m)
        Q_m = Q(X_m)
        T_m = T(X_m)
        w_m = B_m/C_m*dx_m
        d_m = 1./w_m - 1/(np.exp(w_m) - 1)
        W_m = w_m/2.*1./np.sinh(w_m/2.)
        Wp_m = W_m*np.exp(w_m/2.)
        Wm_m = W_m*np.exp(-w_m/2.)
        
        dx_m_ph = X[1] - X[0]
        X_m_ph = 0.5*(X[1] + X[0])
        A_m_ph = A(X_m_ph)
        B_m_ph = B(X_m_ph)
        C_m_ph = C(X_m_ph)
        Q_m_ph = Q(X_m_ph)
        w_m_ph = B_m_ph/C_m_ph*dx_m_ph
        d_m_ph = 1./w_m_ph - 1/(np.exp(w_m_ph) - 1)
        W_m_ph = w_m_ph/2.*1./np.sinh(w_m_ph/2.)
        Wp_m_ph = W_m_ph*np.exp(w_m_ph/2.)
        Wm_m_ph = W_m_ph*np.exp(-w_m_ph/2.)
        
        c_m = dt/(A_m*dx_m)*C_m_ph/dx_m_ph*Wp_m_ph
        b_m = 1 + dt/(A_m*dx_m)*(C_m_mh/dx_m_mh*Wp_m_mh + C_m_ph/dx_m_ph*Wm_m_ph) + dt/T_m
        
        
        a[0] = 0.
        b[0] = b_m
        c[0] = -c_m
        
        r_m = dt*Q_m + u[0]
        R[0][0] = r_m
        
        # Cas xm = M
        
        dx_m    = X[M] - X[M-1]
        X_m = X[M]
        A_m = A(X_m)
        B_m = B(X_m)
        C_m = C(X_m)
        Q_m = Q(X_m)
        T_m = T(X_m)
        w_m = B_m/C_m*dx_m
        d_m = 1./w_m - 1/(np.exp(w_m) - 1)
        W_m = w_m/2.*1./np.sinh(w_m/2.)
        Wp_m = W_m*np.exp(w_m/2.)
        Wm_m = W_m*np.exp(-w_m/2.)
        
    
        dx_m_mh = X[M] - X[M-1]
        X_m_mh = 0.5*(X[M] + X[M-1])
        A_m_mh = A(X_m_mh)
        B_m_mh = B(X_m_mh)
        C_m_mh = C(X_m_mh)
        Q_m_mh = Q(X_m_mh)
        w_m_mh = B_m_mh/C_m_mh*dx_m_mh
        d_m_mh = 1./w_m_mh - 1/(np.exp(w_m_mh) - 1)
        W_m_mh = w_m_mh/2.*1./np.sinh(w_m_mh/2.)
        Wp_m_mh = W_m_mh*np.exp(w_m_mh/2.)
        Wm_m_mh = W_m_mh*np.exp(-w_m_mh/2.)
    
    
        a_m = dt/(A_m*dx_m)*C_m_mh/dx_m_mh*Wm_m_mh
        b_m = 1 + dt/(A_m*dx_m)*(C_m_mh/dx_m_mh*Wp_m_mh + C_m_ph/dx_m_ph*Wm_m_ph) + dt/T_m
        
        
        a[M] = -a_m
        b[M] = b_m
        c[M] = 0.
        
        r_m = dt*Q_m + u[M]
        R[0][M] = r_m
        
        
    #    for ii in range(0, M+1) : 
    #        TT[ii][ii]   = b[ii]
    #        if (ii < M) : 
    #            TT[ii+1][ii] = a[ii]
    #            TT[ii][ii+1] = c[ii]
    #    Tinv = InverseTrigonalMatrix(TT)
    #    U = ProductMatrix(Tinv, R.T)    
        
    #    U = U.T[0]
        
        U = TDMA(a,b,c,R[0])
        u = U
        
        u[0] = 0.
        u[M] = u[M-1]
        
    #    u = U
        
        t += dt 
    
    u_end = u
    
    return u_end



Tmax = 30.*kyr
#dt = 0.1*kyr


u_0 = CC70Solver(0.5*kyr, Tmax, u)
u_1 = CC70Solver(1.*kyr, Tmax, u)
u_2 = CC70Solver(10.*kyr, Tmax, u)



plt.loglog(X/pc-500, u_ini, c="black")

plt.loglog(X/pc-510, u_0, c="black")
plt.loglog(X/pc-500, u_1, c="blue")
plt.loglog(X/pc-500, u_2, c="red")
plt.ylim(1e-16, 1e-9)





