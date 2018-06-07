/**
 *
 *  @file
 *  @author An Tao
 *  @section LICENSE
 *
 *  Copyright 2018, An Tao.  All rights reserved.
 *  Use of this source code is governed by a MIT license
 *  that can be found in the License file.
 *
 *  @section DESCRIPTION
 *
 */

#include "create_controller.h"
#include "cmd.h"
#include <iostream>
#include <fstream>
#include <regex>

using namespace drogon_ctl;

void create_controller::handleCommand(std::vector<std::string> &parameters)
{
    //std::cout<<"create!"<<std::endl;
    ControllerType type=Simple;
    for(auto iter=parameters.begin();iter!=parameters.end();iter++)
    {
        if((*iter)[0]=='-')
        {
            if(*iter=="-s"||(*iter=="--simple"))
            {
                parameters.erase(iter);
                break;
            }
            else if(*iter=="-a"||*iter=="--api")
            {
                type=API;
                parameters.erase(iter);
                break;
            }
            else if(*iter=="-n"||*iter=="--namespace")
            {
                if(type==Simple)
                    break;
                else
                {
                    std::cout<<ARGS_ERROR_STR<<std::endl;
                    return;
                }
            }
            else
            {
                std::cout<<ARGS_ERROR_STR<<std::endl;
                return;
            }
        }
    }
    if(type==Simple)
    {
        std::string namespaceName;
        for(auto iter=parameters.begin();iter!=parameters.end();iter++)
        {
            if((*iter)[0]=='-')
            {
                if(*iter=="-n"||*iter=="--namespace")
                {
                    iter=parameters.erase(iter);
                    if(iter!=parameters.end())
                    {
                        namespaceName=*iter;
                        iter=parameters.erase(iter);
                        break;
                    }
                    else
                    {
                        std::cout<<"please enter namespace"<<std::endl;
                        return;
                    }
                }else
                {
                    std::cout<<ARGS_ERROR_STR<<std::endl;
                    return;
                }
            }

        }
        createSimpleController(parameters,namespaceName);
    }
    else
        createApiController(parameters);
}

void create_controller::createSimpleController(std::vector<std::string> &ctlNames,const std::string &namespaceName)
{
    for(auto iter=ctlNames.begin();iter!=ctlNames.end();iter++)
    {
        if ((*iter)[0] == '-')
        {
            std::cout<<ARGS_ERROR_STR<<std::endl;
            return;
        }
    }
    for(auto ctlName:ctlNames)
    {
        createSimpleController(ctlName,namespaceName);
    }
}
void create_controller::createSimpleController(const std::string &ctlName,const std::string &namespaceName)
{
    std::cout<<"create simple controller:"<<namespaceName<<(namespaceName==""?"":"::")<<ctlName<<std::endl;
    std::string headFileName=ctlName+".h";
    std::string sourceFilename=ctlName+".cc";
    std::ofstream oHeadFile(headFileName.c_str(),std::ofstream::out);
    std::ofstream oSourceFile(sourceFilename.c_str(),std::ofstream::out);
    if(!oHeadFile||!oSourceFile)
        return;
    newSimpleControllerHeaderFile(oHeadFile,ctlName,namespaceName);
    newSimpleControllerSourceFile(oSourceFile,ctlName,namespaceName);
}
void create_controller::newSimpleControllerHeaderFile(std::ofstream &file,const std::string &ctlName,const std::string &namespaceName)
{
    file<<"#pragma once\n";
    file<<"#include <drogon/HttpSimpleController.h>\n";
    file<<"using namespace drogon;\n";
    std::string indent="";
    if(namespaceName!="") {
        file << "namespace " << namespaceName << "{\n";
        indent="    ";
    }
    file<<indent<<"class "<<ctlName<<":public drogon::HttpSimpleController<"<<ctlName<<">\n";
    file<<indent<<"{\n";
    file<<indent<<"public:\n";
        //TestController(){}
    file<<indent<<"    virtual void asyncHandleHttpRequest(const HttpRequest& req,const std::function<void (HttpResponse &)> & callback)override;\n";

    file<<indent<<"    PATH_LIST_BEGIN\n";
    file<<indent<<"    //list path definations here;\n";
    file<<indent<<"    //PATH_ADD(\"/path\",\"filter1\",\"filter2\",...);\n";
    file<<indent<<"    PATH_LIST_END\n";
    file<<indent<<"};\n";
    if(namespaceName!="")
        file<<"}\n";
}
void create_controller::newSimpleControllerSourceFile(std::ofstream &file,const std::string &ctlName,const std::string &namespaceName)
{
    file<<"#include \""<<ctlName<<".h\"\n";
    if(namespaceName!="")
        file<<"using namespace "<<namespaceName<<";\n";
    file<<"void "<<ctlName<<"::asyncHandleHttpRequest(const HttpRequest& req,const std::function<void (HttpResponse &)> & callback)\n";
    file<<"{\n";
    file<<"    //write your application logic here\n";
    file<<"}";
}
void create_controller::createApiController(std::vector<std::string> &apiClasses)
{
    for(auto iter=apiClasses.begin();iter!=apiClasses.end();iter++)
    {
        if ((*iter)[0] == '-')
        {
            std::cout<<ARGS_ERROR_STR<<std::endl;
            return;
        }
    }
    for(auto className:apiClasses)
    {
        createApiController(className);
    }
}
void create_controller::createApiController(const std::string &className)
{
    std::regex regex("::");
    std::string ctlName=std::regex_replace(className,regex,"_");

    std::cout<<"create api controller:"<<className<<std::endl;
    std::string headFileName=ctlName+".h";
    std::string sourceFilename=ctlName+".cc";
    std::ofstream oHeadFile(headFileName.c_str(),std::ofstream::out);
    std::ofstream oSourceFile(sourceFilename.c_str(),std::ofstream::out);
    if(!oHeadFile||!oSourceFile)
        return;
    newApiControllerHeaderFile(oHeadFile,className);
    newApiControllerSourceFile(oSourceFile,className,ctlName);
}

void create_controller::newApiControllerHeaderFile(std::ofstream &file,const std::string &className)
{
    file<<"#pragma once\n";
    file<<"#include <drogon/HttpApiController.h>\n";
    file<<"using namespace drogon;\n";
    std::string indent="";
    std::string class_name=className;
    std::string namepace_path="/";
    auto pos=class_name.find("::");
    while(pos!=std::string::npos)
    {
        auto namespaceName=class_name.substr(0,pos);
        class_name=class_name.substr(pos+2);
        file<<indent<<"namespace "<<namespaceName<<"\n";
        namepace_path.append(namespaceName).append("/");
        file<<indent<<"{\n";
        indent.append("    ");
        pos=class_name.find("::");
    }
    file<<indent<<"class "<<class_name<<":public drogon::HttpApiController<"<<class_name<<">\n";
    file<<indent<<"{\n";
    file<<indent<<"public:\n";
    indent.append("    ");
    file<<indent<<"METHOD_LIST_BEGIN\n";
    file<<indent<<"//use METHOD_ADD to add your custom processing function here;\n";
    file<<indent<<"//METHOD_ADD("<<class_name<<"::get,\"/{2}/{1}\",1,\"drogon::GetFilter\");"
                                "//path will be "<<namepace_path<<class_name<<"/get/{arg2}/{arg1}\n";
    file<<indent<<"//METHOD_ADD("<<class_name<<"::your_method_name,\"/{1}/{2}/list\",0,\"drogon::GetFilter\");"
                  "//path will be "<<namepace_path<<class_name<<"/{arg1}/{arg2}/list\n";
    file<<indent<<"\n";
    file<<indent<<"METHOD_LIST_END\n";
    file<<indent<<"//your declaration of processing function maybe like this:\n";
    file<<indent<<"//void get(const HttpRequest& req,"
                  "const std::function<void (HttpResponse &)>&callback,int p1,std::string p2);\n";
    file<<indent<<"//void your_method_name(const HttpRequest& req,"
                  "const std::function<void (HttpResponse &)>&callback,double p1,int p2) const;\n";
    indent.resize(indent.length()-4);
    file<<indent<<"};\n";
    if(indent=="")
        return;
    do {
       indent.resize(indent.length()-4);
       file<<indent<<"}\n";
    } while(indent!="");
}
void create_controller::newApiControllerSourceFile(std::ofstream &file,const std::string &className,const std::string &filename)
{
    file<<"#include \""<<filename<<".h\"\n";
    auto pos=className.rfind("::");
    auto class_name=className;
    if(pos!=std::string::npos)
    {
        auto namespacename=className.substr(0,pos);
        file<<"using namespace "<<namespacename<<";\n";
        class_name=className.substr(pos+2);
    }

    file<<"//add defination of your processing function here\n";
}