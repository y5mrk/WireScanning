/**
* @file SvgDrawer.hpp
* @author Takashi Michikawa <michikawa@acm.org>
*/
#ifndef MI_SVG_DRAWER_HPP
#define MI_SVG_DRAWER_HPP 1
#include <fstream>
#include <string>
#include <iostream>
#include <cmath>
namespace mi
{
        /**
        * @class SvgDrawer "SvgDrawer.hpp" <mi/SvgDrawer.hpp>
        * @brief Light-weight drawing tools of vector graphics as SVG files.
        * @note Current implementation supports drawing lines, circles and rectangles.
        */
        class SvgDrawer
        {
        private:
                class Vector2d
                {
                public:
                        double x ;
                        double y ;
                private:
                        Vector2d( const Vector2d& that );
                        void operator = ( const Vector2d& that );
                public:
                        explicit Vector2d ( const double x0 = 0, const double y0 = 0 ) : x( x0 ), y( y0 ) {
                                return;
                        }
                };
        private:
                std::ofstream _fout;
                int _stroke_dashed;
                double _stroke_width;
                std::string _stroke_color;
                std::string _fill_color;

                SvgDrawer::Vector2d _bmin;
                SvgDrawer::Vector2d _bmax;
                SvgDrawer::Vector2d _size;
        public:
                SvgDrawer( const int width, const int height, const int view_w, const int view_h, const std::string& filename = std::string( "draw.svg" ) ) {
                        this->init();
                        this->_size.x = width;
                        this->_size.y = height;
                        this->setViewBox( 0, 0, view_w, view_h );
                        this->_fout.open( filename.c_str() );

                    this->_fout<<"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n<svg version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" preserveAspectRatio=\"xMinYMin slice\" width= \""<<width<<"mm\" height=\""<<height<<"mm\" viewBox=\"0 0 "<<view_w<<" "<<view_h<<"\" x=\"0mm\" y=\"0mm\" overflow=\"visible\" >"<<std::endl;
                        return;
                }

                ~SvgDrawer ( void ) {
                        this->_fout<<"</svg>"<<std::endl;
                        return;
                }

                void setViewBox ( const double mnx, const double mny, const double mxx, const double mxy ) {
                        if ( std::fabs( mxx - mnx ) < 1.0e-10 || std::fabs( mxy - mny ) < 1.0e-10 ) {
                                std::cerr<<"warning : invalid viewport size"<<"("<<mnx<<","<<mny<<")"<<"-"<<"("<<mxx<<","<<mxy<<")"<<std::endl;
                        }
                        this->_bmin.x =  mnx;
                        this->_bmin.y =  mny;
                        this->_bmax.x =  mxx;
                        this->_bmax.y =  mxy;
                        return;
                }


                void init ( void ) {
                        this->_stroke_dashed = 0;
                        this->setStrokeWidth( 1.0 );
                        this->setStrokeColor( std::string( "#000000" ) );
                        this->setFillColor( std::string( "#000000" ) );
                }

                void setStrokeWidth ( const double width ) {
                        this->_stroke_width = width;
                }

                void setStrokeColor( const std::string &col ) {
                        this->_stroke_color = col;
                }

                void setFillColor( const std::string &col ) {
                        this->_fill_color = col;
                }

                void setStrokeDash( const int dash_pitch ) {
                        this->_stroke_dashed = dash_pitch;
                }

                void drawLine ( const double x0 , const double y0, const double x1, const double y1 ) {
                        SvgDrawer::Vector2d v0( x0, y0 );
                        SvgDrawer::Vector2d v1( x1, y1 );
                        this->convert( v0 );
                        this->convert( v1 );
                        this->_fout<<"<line x1=\""<<v0.x<<"\" y1=\""<<v0.y<<"\" x2=\""<<v1.x<<"\" y2=\""<<v1.y<<"\"";
                        if ( this->_stroke_dashed > 0 ) this->_fout<<" stroke-dasharray=\""<<this->_stroke_dashed<<"\"";
                        this->_fout<<" stroke-width=\""<<this->_stroke_width<<"\" stroke=\""<<this->_stroke_color<<"\" />"<<std::endl;
                        return;
                }

                void drawCircle ( const double cx, const double cy, const double r ) {
                        SvgDrawer::Vector2d v0( cx, cy );
                        this->convert( v0 );
                        this->_fout<<"<circle cx=\""<<v0.x<<"\" cy=\""<<v0.y<<"\" r=\""<<r<<"\""
                                   <<" stroke-width=\""<<this->_stroke_width<<"\""
                                   <<" stroke=\""<<this->_stroke_color<<"\"";
                        if ( this->_stroke_dashed > 0 ) this->_fout<<" stroke-dasharray=\""<<this->_stroke_dashed<<"\"";
                        this->_fout<<" fill=\""<<this->_fill_color<<"\"/>"<<std::endl;
                        return;
                }

                void drawRect ( const double x0, const double y0, const double w, const double h ) {
                        SvgDrawer::Vector2d v0( x0, y0 );
                        SvgDrawer::Vector2d v1( w+x0,h+y0 );
                        this->convert( v0 );
                        this->convert( v1 );

                        double minx = v0.x < v1.x ? v0.x : v1.x;
                        double miny = v0.y < v1.y ? v0.y : v1.y;
                        double maxx = v0.x > v1.x ? v0.x : v1.x;
                        double maxy = v0.y > v1.y ? v0.y : v1.y;
                        double sizex = maxx - minx;
                        double sizey = maxy - miny;
                        this->_fout<<"<rect x=\""<<minx<<"\" y=\""<<miny<<"\" width=\""<<sizex<<"\" height=\""<<sizey<<"\"";
                        if ( this->_stroke_dashed > 0 ) this->_fout<<" stroke-dasharray=\""<<this->_stroke_dashed<<"\"";
                        this->_fout<<" stroke-width=\""<<this->_stroke_width<<"\" stroke=\""<<this->_stroke_color<<"\" fill=\""<<this->_fill_color<<"\"/>"<<std::endl;

                }
            
                void PolyLine ( const std::vector< cv::Point > polypoints, double SVGratio) {
                    this->_fout<<"<polyline points=\"";
                    for (int i = 0; i < polypoints.size()-1; ++i){
                        SvgDrawer::Vector2d v0( polypoints[i].x / SVGratio, polypoints[i].y / SVGratio );
//                        this->convert( v0 );
                        this->_fout<<v0.x<<" "<<v0.y<<",";
                    }
                    SvgDrawer::Vector2d v0( polypoints[polypoints.size()-1].x / SVGratio , polypoints[polypoints.size()-1].y / SVGratio );
//                    this->convert( v0 );
                    this->_fout<<v0.x<<" "<<v0.y<<"\"";
//                    zthis->_fout<<"\"";
                    if ( this->_stroke_dashed > 0 ) this->_fout<<" stroke-dasharray=\""<<this->_stroke_dashed<<"\"";
                    this->_fout<<" style=\"fill:none;stroke:"<<this->_stroke_color<<";stroke-width:"<<this->_stroke_width<<";\" />"<<std::endl;
//                    this->_fout<<" stroke-width=\""<<this->_stroke_width<<"\" stroke=\""<<this->_stroke_color<<"\" />"<<std::endl;
                    return;
                }
            
                //スプライン曲線 作りかけ
                void splineCurve ( const std::vector< cv::Point > polypoints) {
                    this->_fout<<"<path d=\"";
                    SvgDrawer::Vector2d v0( polypoints[0].x , polypoints[0].y );
                    this->convert( v0 );
                    this->_fout<<"M"<<v0.x<<","<<v0.y<<" ";

                    for (int i = 1; i < polypoints.size()-1; ++i){
                        SvgDrawer::Vector2d v0( polypoints[i].x, polypoints[i].y );
                        this->convert( v0 );
                        this->_fout<<"T"<<v0.x<<","<<v0.y<<" ";
                    }
                    SvgDrawer::Vector2d v1( polypoints[polypoints.size()-1].x, polypoints[polypoints.size()-1].y );
                    this->convert( v1 );
                    this->_fout<<"T"<<v1.x<<" "<<v1.y<<" z\"";
                //                    zthis->_fout<<"\"";
                    if ( this->_stroke_dashed > 0 ) this->_fout<<" stroke-dasharray=\""<<this->_stroke_dashed<<"\"";
                    this->_fout<<" style=\"fill:none;stroke:"<<this->_stroke_color<<";stroke-width:"<<this->_stroke_width<<";\" />"<<std::endl;
                //                    this->_fout<<" stroke-width=\""<<this->_stroke_width<<"\" stroke=\""<<this->_stroke_color<<"\" />"<<std::endl;
                    return;
                }
        private:
                void convert ( Vector2d& p ) {
                        const double s = ( p.x - this->_bmin.x ) / ( this->_bmax.x - this->_bmin.x ) ;
                        const double t = ( p.y - this->_bmin.y ) / ( this->_bmax.y - this->_bmin.y ) ;
                        p.x = s * this->_size.x;
                        p.y = ( 1.0 - t ) * this->_size.y;
                        return;
                }
        };
}
#endif// MI_SVG_DRAWER_HPP
