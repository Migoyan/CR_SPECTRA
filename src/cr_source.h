#include <iostream> //For console output/input
#include <fstream>  //Allows to read/write files 
#include <math.h>   //Basic mathematic functions 
#include <vector>   //For dynamic arrays 
#include <string>   //Operations on strings 
#include <tuple>
#include <cmath>
#include <sstream>
using namespace std;


std::string parameters = "./parameters.dat";
std::string snx = search(parameters, "NX"); int nx = stoi(snx);
std::string sne = search(parameters, "NE"); int ne = stoi(sne);
std::string sni = search(parameters,"ni");  double ni = stod(sni);
std::string sX  = search(parameters,"X");   double Xi = stod(sX); 
double nt = ni/Xi;
std::string smn = search(parameters,"mn");  double m_neutral = stod(smn); 
std::string sT   = search(parameters,"T");  double T  = stod(sT);
std::string scenter = search(parameters, "center"); double x_center = stod(scenter);
std::string scenter_index = search(parameters, "center_index"); int x_center_index = stod(scenter_index);
std::string sBcenter = search(parameters, "B"); double Bcenter = stod(sBcenter);





double GetTSed()
    {
        double tfree  = 0.3*pow(Esn,-0.5)*Mej*pow(nt,-1./3)*kyr; // [s]
        return tfree;
    }




// R_sh function 
double RSNR(double time)
    {
        //###############################################################################
        //# FUNCTIONS IN ORDER TO MAKE OUR SNR EXPAND IN THE ISM                        #
        //###############################################################################
        double vej8  = 10.*pow(Esn/Mej, 0.5);

        //# We define the characteristic times of our problem
        double tini   = 1e-4*kyr; // [s]
        double tfree  = 0.3*pow(Esn,-0.5)*Mej*pow(nt,-1./3)*kyr; // [s]
        double tPDS   = exp(-1.)*3.61e4*pow(Esn,3./14)/(pow(xi_n,5./14)*pow(nt,4./7))*yr; // [s]
        double tMCS   = min(61*pow(vej8,3)/(pow(xi_n,9./14)*pow(nt,3./7)*pow(Esn,3./14)), 476./pow(xi_n*phi_c,9./14))*tPDS; // [s]
        double tmerge = 153.*pow(pow(Esn,1./14)*pow(nt,1./7)*pow(xi_n,3./14)/(bbeta*C06), 10./7)*tPDS; // [s]
        double tmax = min(tMCS, tmerge); // [s]

        // We define the characteristic radii of our problem
        vector<double> t;
        vector<double> r;
        double R_free  = 5.0*pow(Esn/nt,1./5)*pow(1 - (0.05*pow(Mej,5./6))/(pow(Esn,0.5)*pow(nt,1./3)*(tfree/kyr)), 2./5)*pow(tfree/kyr,2./5)*pc; // [cm]
        double R_ini   = R_free*(tini/tfree); // [cm]
        double R_PDS   = 5.0*pow(Esn/nt,1./5)*pow(1 - (0.05*pow(Mej,5./6))/(pow(Esn,0.5)*pow(nt,1./3)*(tPDS/kyr)), 2./5)*pow(tPDS/kyr,2./5)*pc; // [cm]
        double R_MCS   = R_PDS*pow(tMCS/tPDS, 3./10); // [cm]
        double R_merge = R_MCS*pow(tmerge/tMCS, 1./4); // [cm]
        if (tMCS < tmerge)
        {
            t.push_back(1e-6*kyr); 
            t.push_back(tini); 
            t.push_back(tfree); 
            t.push_back(tPDS); 
            t.push_back(tMCS); 
            t.push_back(tmerge); 
            //t = {1e-6*kyr, tini, tfree, tPDS, tMCS, tmerge};
            r.push_back(1e-6*pc);
            r.push_back(R_ini);
            r.push_back(R_free);
            r.push_back(R_PDS);
            r.push_back(R_MCS);
            r.push_back(R_merge);
            //r = {1.e-6*pc ,R_ini, R_free, R_PDS, R_MCS, R_merge};
        }
        if (tMCS >= tmerge)
        {
            t.push_back(1e-6*kyr); 
            t.push_back(tini); 
            t.push_back(tfree); 
            t.push_back(tPDS); 
            t.push_back(tmerge);
            //t = {1e-6*kyr ,tini, tfree, tPDS, tmerge};
            r.push_back(1e-6*pc);
            r.push_back(R_ini);
            r.push_back(R_free);
            r.push_back(R_PDS);
            r.push_back(R_merge);
            //r = {1.e-6*pc, R_ini, R_free, R_PDS, R_merge};
        }
    
        vector<double> logt; logt.resize(t.size());
        vector<double> logr; logr.resize(r.size());
        for (int i = 0; i < logt.size(); i++)
        {
            //cout<<"log10(t[i]) = "<<log10(t[i])<<endl;
            //cout<<"log10(r[i]) = "<<log10(r[i])<<endl;
            logt[i] = log10(t[i]);
            logr[i] = log10(r[i]);
        }

        double logr_new, r_new; 
        double logtime = log10(time);
        logr_new = InterpolatingSpline(logt, logr, logtime);
        //cout<<"log_rnew = "<<logr_new<<endl; //-----------------------------------------------------------------------------------
        r_new = pow(10, logr_new);
        //cout<<"r_new = "<<logr_new<<endl;

        //{cout<<"r_new = "<<r_new; cout<<"logr_new = "<<logr_new<<endl;}

        if (isnan(r_new) ){return 1e-6*pc;}
        if (!(isnan(r_new))){return r_new;}
    }


// Shock velocity in time 
double u_sh(double time)
    {
        double dt = time/1e4;
        double R_sup = RSNR(time+dt);
        double R_inf = RSNR(time-dt);
        return (R_sup - R_inf)/(2*dt);
    }


// Get the EMAX value for the tesc calculation 
double GetEM()
    {
        int precision = 1000;
        vector<double> Emax; Emax.resize(precision);
        double eps = 1e-4;
        double x0 = 10.*GeV;
        double aa,bb,cc;
        double bbeta = gam-2;
        vector<double> cst; cst.resize(3);

        vector<double> loc_time; loc_time.resize(precision);
        vector<double> log_time; log_time.resize(precision);
        double tmin = 0.01*yr;
        double tmax = 2000*kyr;

        for (int i = 0; i < precision; i++)
            {
                log_time[i] = log10(tmin) + i*(log10(tmax) - log10(tmin))/precision;
                loc_time[i] = pow(10, log_time[i]);
            }





        if (gam-2 != 0)
            {
                for (int i = 0; i < precision; i++)
                    {
                        aa = Emin;
                        bb = bbeta;
                        cc = (bbeta/(1+bbeta))*e*sqrt(4*pi*nt*mp)/(10*c)*xhi_cr*pow(u_sh(loc_time[i]),2)*RSNR(loc_time[i]);
                        //cout<<"cc = "<<RSNR(loc_time[i])<<endl;
                        cst[0] = aa; cst[1] = bb; cst[2] = cc;
                        Emax[i] = NewtonRaphson(f2, df2dx, x0, eps, cst);
                        //cout<<"Emax = "<<Emax[i]/GeV<<" GeV"<<endl; //----------------------------------------------------------------------
                    }

            }
        if (gam-2 == 0)
            {
                for (int i = 0; i < precision; i++)
                    {
                        aa = Emin;
                        bb = e*sqrt(4*pi*nt*mp)/(10*c)*xhi_cr*pow(u_sh(loc_time[i]),2.)*RSNR(loc_time[i]);
                        cst[0] = aa; cst[1] = bb;
                        Emax[i] = NewtonRaphson(f1, df1dx, x0, eps, cst);
                    }
            }
        
        double EM = GetMax(Emax);
        return EM;
    }





// CRs Escape time function (according to Celli et al. 2019) 
/*double tesc(double E)
    {
        double tSed = GetTSed();
        double EM = GetEM();
        double ddelt = -1./(delta); 

        //cout<<"EM = "<<EM/GeV<<" GeV"<<endl;

        //tSed*((E**2/cst.c**2 - cst.mp**2*cst.c**2)/(EMAX**2/cst.c**2 - cst.mp**2*cst.c**2))**(-1./(2.*delta))

        
        //cout<<"(pow(E/c,2) - pow(mp*c,2)) = "<<(pow(E/c,2) - pow(mp*c,2))<<endl;
        //cout<<"(pow(EM/c,2) - pow(mp*c,2)) = "<<(pow(EM/c,2) - pow(mp*c,2))<<endl;
        //cout<<"(pow(E/c,2) - pow(mp*c,2))/(pow(EM/c,2) - pow(mp*c,2) = "<<( pow(E/c,2) - pow(mp*c,2) )/( pow(EM/c,2) - pow(mp*c,2) )<<endl;
        //cout<<"pow( (pow(E/c,2) - pow(mp*c,2)) / (pow(EM/c,2) - pow(mp*c,2)) , ddelt) = "<<pow( ( pow(E/c,2) - pow(mp*c,2) ) / (pow(EM/c,2) - pow(mp*c,2) ) , ddelt )<<endl;
        //cout<<"-1./(2.*delta) = "<<-1./(2.*delta)<<endl;
        //cout<<"E = "<<E/GeV<<" GeV, tesc = "<<tSed*pow( (pow(E/c,2) - pow(mp*c,2)) / (pow(EM/c,2) - pow(mp*c,2)) , ddelt )/kyr<<" kyr"<<endl;
        return tSed*pow( (pow(E/c,2) - pow(mp*c,2)) / (pow(EM/c,2) - pow(mp*c,2)) , ddelt );
    }*/


double tesc(double E)
    {
        double tSed = GetTSed();
        double EM = GetEM();
        double ddelt = -1./(delta); 

        double tPDS   = exp(-1.)*3.61e4*pow(Esn,3./14)/(pow(xi_n,5./14)*pow(nt,4./7))*yr; // [s]
        
        //double loc_tesc = tSed*pow( (pow(E/c,2) - pow(mp*c,2)) / (pow(EM/c,2) - pow(mp*c,2)) , ddelt);
        double loc_tesc = tSed*pow( (pow(E/c,2)) / (pow(EM/c,2)) , ddelt);
        double tfree  = 0.3*pow(Esn,-0.5)*Mej*pow(nt,-1./3)*kyr; // [s]

        double v_sh = u_sh(loc_tesc);

        double t_sh_lib   = tSed;
        double v_sh_110 = u_sh(t_sh_lib); 
        double delta_t    = 100.*yr;
        // We calculate the time at which u_sh = 110 km/s 
        while(v_sh_110 > 110*km)
        {
            t_sh_lib += delta_t;
            v_sh_110 = u_sh(t_sh_lib); 
        }

        if (E > EM || E < Emin)
        {
            if (injection_cutoff == 0){return pow(10,20)*kyr;}
            if (injection_cutoff == 1){return loc_tesc;}
        }



        if (tesc_model == 0) // no radiative escape model 
        {
            return loc_tesc;
        }
        if (tesc_model == 1) // if t >= tPDS
        {
            return min(tPDS, loc_tesc); 
        }
        if (tesc_model == 2) // if vsh <= 110 km.s^-1
        {
            if (loc_tesc > tPDS)
            {
                return t_sh_lib;
                //if (v_sh <= 110*km) 
                //    {
                        //double loc_time = loc_tesc; 
                        //double delta_time = 100.*yr;
                        //double loc_vsh = u_sh(loc_time);
                        //while(loc_vsh < 110*km)
                        //{
                        //    loc_time -= delta_time; 
                        //    loc_vsh = u_sh(loc_time); 
                        //    //cout<<"loc_time = "<<loc_time/kyr<<" kyr, loc_vsh = "<<loc_vsh/km<<" km/s"<<endl;
                        //}
                        //return loc_time; 
                //        return t_sh_lib; 
                //    }
                //if (v_sh > 110*km)  {return t_sh_lib;} // We return the time at which v_sh will be equal to 110 km/s
            }
            else 
            {
                return loc_tesc;
            }
        }
    }


// Old-function, useless ... 
double Bsat_old(vector<vector<double> > &Pcr, vector<double> X, vector<double> Y, double r_snr)
    {
        double dX = X[1] - X[0]; 
        double dY = Y[1] - Y[0];
        double shock_pos = (x_center+r_snr)/dX;
        int index_pos_sh = int (shock_pos);

        double Int = 0.; 
        for (int ei = 0; ei < Y.size(); ei++)
        {
            //cout<<Pcr[index_pos_sh][ei]*log(10.)*pow(10., Y[ei])*abs(dY)<<endl;
            Int += Pcr[index_pos_sh][ei]*log(10.)*pow(10., Y[ei])*abs(dY); 
        }

        double B_sat = pow(24*pi*Int,0.5);
        
        //cout<<"x_center = "<<x_center<<", r_snr = "<<r_snr<<", shoxk_pos = "<<shock_pos;
        //cout<<", index_pos_shock = "<<index_pos_sh;
        //cout<<", Int = "<<Int<<", Bsat = "<<B_sat<<endl; 

        return B_sat;
    }


double B_sat(double time)
    {
        double B_ISM = Bcenter;

        // See Ohira et al. (2012), eq. 20
        double t_sed  = 0.3*pow(Esn,-0.5)*Mej*pow(nt,-1./3)*kyr; // [s]
        double R_sed  = 5.0*pow(Esn/nt,1./5)*pow(1 - (0.05*pow(Mej,5./6))/(pow(Esn,0.5)*pow(nt,1./3)*(t_sed/kyr)), 2./5)*pow(t_sed/kyr,2./5)*pc; // [cm]
        
        double Bfree = eta_gfree*eta_acc*c*t_sed*Eknee/(3*e*pow(R_sed,2));

        //cout<<"Bfree = "<<Bfree*1e6<<endl; 

        double alpha_B, t_B;
        if (oh_model == 1){alpha_B = alpha - 1./5;}
        if (oh_model == 2){alpha_B = 9./10;}
        if (oh_model == 3){alpha_B = 3./5;}

        t_B = t_sed*pow(Bfree/B_ISM, 1./alpha_B);

        //cout<<"tesc_p = "<<time/kyr<<" kyr"<<endl;

        //cout<<Bfree<<", "<<B_ISM<<", "<<Bfree*pow(time/t_sed, -alpha_B)<<endl;

        if (time <= t_sed)              {return Bfree;}
        if (time > t_sed & time <= t_B) {return Bfree*pow(time/t_sed, -alpha_B);}
        if (time > t_B)                 {return B_ISM;}
    }


double GetEM_e()
    {
        double t_sed  = 0.3*pow(Esn,-0.5)*Mej*pow(nt,-1./3)*kyr; // [s]
        double Bsat = B_sat(t_sed);
        double EM = 9*pow(me, 4)*pow(c, 7)/(4*16*pow(e, 4)*pow(Bsat,2)*t_sed);
        //cout<<"EM electrons = "<<EM/GeV<<endl;
        double EM_p = GetEM();

        return min(EM, EM_p);
    }

double tesc_e(double E)
    {
        double tSed = GetTSed();
        double EM = GetEM_e();
        double EM_p = GetEM();
        double ddelt = -1./(delta); 

        double tPDS   = exp(-1.)*3.61e4*pow(Esn,3./14)/(pow(xi_n,5./14)*pow(nt,4./7))*yr; // [s]
        
        //double loc_tesc = tSed*pow( (pow(E/c,2) - pow(mp*c,2)) / (pow(EM/c,2) - pow(mp*c,2)) , ddelt);
        double loc_tesc = tSed*pow( (pow(E/c,2)) / (pow(EM_p/c,2)) , ddelt);
        double tfree  = 0.3*pow(Esn,-0.5)*Mej*pow(nt,-1./3)*kyr; // [s]

        double v_sh = u_sh(loc_tesc);

        double t_sh_lib   = tSed;
        double v_sh_110 = u_sh(t_sh_lib); 
        double delta_t    = 100.*yr;
        // We calculate the time at which u_sh = 110 km/s 
        while(v_sh_110 > 110*km)
        {
            t_sh_lib += delta_t;
            v_sh_110 = u_sh(t_sh_lib); 
        }

        if (E > EM || E < Emin)
        {
            if (injection_cutoff == 0){return pow(10,20)*kyr;}
            if (injection_cutoff == 1){return pow(10,20)*kyr;}
        }


        if (tesc_model == 0) // no radiative escape model 
        {
            return loc_tesc; 
        }
        if (tesc_model == 1) // if t >= tPDS
        {
            return min(tPDS, loc_tesc); 
        }
        if (tesc_model == 2) // if vsh <= 110 km.s^-1
        {
            if (loc_tesc > tPDS)
            {
                return t_sh_lib;
                //if (v_sh <= 110*km) 
                //    {
                        //double loc_time = loc_tesc; 
                        //double delta_time = 100.*yr;
                        //double loc_vsh = u_sh(loc_time);
                        //while(loc_vsh < 110*km)
                        //{
                        //    loc_time -= delta_time; 
                        //    loc_vsh = u_sh(loc_time); 
                        //    //cout<<"loc_time = "<<loc_time/kyr<<" kyr, loc_vsh = "<<loc_vsh/km<<" km/s"<<endl;
                        //}
                        //return loc_time; 
                //        return t_sh_lib; 
                //    }
                //if (v_sh > 110*km)  {return t_sh_lib;} // We return the time at which v_sh will be equal to 110 km/s
            }
            else 
            {
                return loc_tesc;
            }
        }
    }


/*double tesc_e(double E)
    {
        double t_sed  = 0.3*pow(Esn,-0.5)*Mej*pow(nt,-1./3)*kyr; // [s]
        double tesc_p = tesc(E);
        double Bsat  = B_sat(tesc_p); 
        double EM = GetEM();
        //cout<<"tesc_p = "<<tesc_p/kyr<<" kyr, Bsat = "<<Bsat*1e6<<" muG"<<endl;
        //double tloss_e = 4*E/(c*sig_T*pow(Bsat,2)*pow(1 + E/(me*pow(c,2)),2));
        double tloss_e = 9*pow(me, 4)*pow(c, 7)/(4*16*pow(e, 4)*pow(Bsat,2)*E);


        //cout <<"tloss_e = "<<tloss_e/kyr<<" kyr"<<endl;

        if (E > EM || E < Emin){return pow(10,20)*kyr;}

        return max(min(tesc_p, tloss_e), t_sed);
    }*/





//####################################################################################################//

double Resc(double E)
{
    double A = tesc(E); 
    return RSNR(A);
}

double sigm(double E, double ttesc)
{
    return ttesc/injection_function_width;           // Corresponds approximately to the width of the escape time divided
}


// CRs time injection function. Very important ! 
double Finj(double t, double dt, double E, double ttesc)
{
    if (injection_shape_time == 0) // Dirac type injection 
    {
        if (ttesc > t - dt/2. && ttesc < t + dt/2.){
            if (verbose == 1) {cout<<"E = "<<E/GeV<<" GeV, t_min = "<<(t - dt/2.)/kyr<<" < "<<ttesc/kyr<<" < t_max = "<<(t + dt/2.)/kyr<<" kyr"<<endl;} 
            return 1./dt; }
        return 0.;
    }

    if (injection_shape_time == 1) // Gaussian type injection 
    {
        double A = exp(-0.5*pow((t - ttesc)/sigm(E, ttesc),2));
        //double A = exp(-0.5*pow((t - 0.01*kyr)/(0.001*kyr),2));
        double B;

        double time_min = t_start_injection; 
        double time_max = t_end_injection*ttesc;// - time_min;

        int Na = injection_function_norm;                              // Constant in order to easily and rapidly normalize the injection function  
        int Nc = int((time_max - time_min)/dt);

        double ratio = double(Nc)/double(Na);

        double C_a = 0;
        double loc_dt = (time_max - time_min)/double(Na); 
        for (int ti = 0; ti < Na; ti++)
        {
            //C_a += exp(-0.5*pow((t - ttesc)/sigm(E, ttesc),2));
            C_a += exp(-0.5*pow((time_min + ti*loc_dt - ttesc)/sigm(E, ttesc),2))*loc_dt;

        } 
        
        //cout<<ratio<<endl;

        if (C_a == 0){return 0.;}
        if (C_a > 0) 
        {
            //B = A/(C_a*ratio); 
            B = A/C_a;
            if (A > C_a*ratio) {return 0.;}
            if (A < C_a*ratio) {return B;}
        }
    }
}




/*double Finj(double t, double E, double ni, double X)
{
    double A = exp(-0.5*pow((t - tesc(E, ni, X))/sigm(E, ni, X),2));

    vector<double> time(1000); 
    double time_0   = 1e-6*kyr; 
    double time_max = 1e4*kyr;
    for (int i = 0; i<time.size(); i++)
    {
        time[i] = time_0 + (time_max - time_0)*i/time.size(); 
    }
    double intA = 0;
    for (int i = 1; i<time.size(); i++)
    {
        intA = intA + exp(-0.5*pow((time[i] - tesc(E, ni, X))/sigm(E, ni, X),2))*(time[i] - time[i-1]); 
    }
    //cout<<intA<<endl;
    //cout<<A/intA<<endl;
    if (intA > 0){return A/intA;}
    if (intA == 0){return 0;}
    //return A/intA;
}*/

/*double theta(double z, double t, double Rsnr)
{
    //double Rsnr = RSNR(t);
    

    //cout<<Rsnr/pc<<endl;
    if (z < Rsnr){return 1;}
    if (z > Rsnr){return 0;}
}*/

double theta(double z, double t, double Rsnr)
{
    //double Rsnr = RSNR(t);
    double r_width = Rsnr/r_snr_thickness;
    double center  = x_center;
    double loc_z = z - center;

    //return (1 - tanh((z - Rsnr)/r_width))/2.;
    return 0.5*(erf((Rsnr - loc_z)/(r_width)) + erf((Rsnr + loc_z)/(r_width)));
}

double dNdE(double E)
{
    double spec;
    double Emax = GetEM();
    if (gam != 2)
    {
        if (injection_cutoff == 0){spec = (2 - gam)*Esn*xhi_cr*1e51/(pow(Emax,2-gam) - pow(Emin,2-gam))*pow(E,-gam);}
        if (injection_cutoff == 1){spec = (2 - gam)*Esn*xhi_cr*1e51/(pow(Emax,2-gam) - pow(Emin,2-gam))*pow(E,-gam)*exp(-inj_exp_alpha*E/Emax);}
        
    }
    if (gam == 2)
    {
        if (injection_cutoff == 0){spec = Esn*xhi_cr/(log(Emax) - log(Emin))*pow(E,-gam);}
        if (injection_cutoff == 1){spec = Esn*xhi_cr/(log(Emax) - log(Emin))*pow(E,-gam)*exp(-inj_exp_alpha*E/Emax);}
    }
    return spec;
}

double ff(double E)
{
    double R = Resc(E);
    double A = 3*pow(c, 3.)/(16*pow(pi, 2.)*pow(R, 3.))/pow(E,2)*dNdE(E);
    return A;
}

double Pcr_ini(double E)
{
    double A = 4*pi/(3*pow(c, 3))*pow(E, 4.)*ff(E); 
    return A;
}








