/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkOnikaWriter.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkOnikaWriter.h"
#include "vtkObjectFactory.h"

#include "vtkDataArrayWrapper.h"

vtkStandardNewMacro(vtkOnikaWriter);

//----------------------------------------------------------------------------
vtkOnikaWriter::vtkOnikaWriter()
{
}

//----------------------------------------------------------------------------
vtkOnikaWriter::~vtkOnikaWriter()
{
}

//----------------------------------------------------------------------------
void vtkOnikaWriter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

