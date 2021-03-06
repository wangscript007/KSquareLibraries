#if  !defined(_FACTORYFVPRODUCER_)
#define  _FACTORYFVPRODUCER_


#include "RunLog.h"


namespace KKMLL 
{

#if  !defined(_FeatureVector_Defined_)
  class  FeatureVector;
  typedef  FeatureVector*    FeatureVectorPtr;
#endif


#if  !defined(_FeatureVectorList_Defined_)
  class  FeatureVectorList;
  typedef  FeatureVectorList*    FeatureVectorListPtr;
#endif


#if  !defined(_FeatureVectorProducer_Defined_)
  class  FeatureVectorProducer;
  typedef  FeatureVectorProducer*    FeatureVectorProducerPtr;
#endif


#if  !defined(_FeatureFileIO_Defined_)
  class  FeatureFileIO;
  typedef  FeatureFileIO* FeatureFileIOPtr;
#endif


#if  !defined(_FileDesc_Defined_)
  class  FileDesc;
  typedef  FileDesc*  FileDescPtr;
  typedef  FileDesc const FileDescConst;
  typedef  FileDescConst * FileDescConstPtr;
#endif


#if  !defined(_TrainingConfiguration2_Defined_)
  class  TrainingConfiguration2;
  typedef  TrainingConfiguration2*  TrainingConfiguration2Ptr;
#endif


  /**
   *@class  FactoryFVProducer
   *@brief  Responsible for creating a FeatureFectorProducer instance.
   *@details  
   * To make the 'KKMLL' library independent of the actual details of the FeatureVetir computations and 
   * their underlying source data a Factory - Producer strategy was implemented. For every FeatureVector derived class
   * there will be at least one 'FeatureVectorProducer' derived class and one 'FactoryFVProducer' derived class.
   *
   * -FeatureVectorProducer  Responsible for computing instances of 'FeatureVector' derived classes from source data
   *  such as a "Raster" instance.
   * -FactoryFVProducer  Responsible for creating new instances of the 'FeatureVectorProducer' derived classes.  Will
   *  also provide the appropriate 'FileDesc' instance that describes the features computed and Instantiate new
   *  "FeatureVectorList" derived instances for containing the produced FeatureFevtor derived instances.
   *
   *  New FeatureVector instances will be computed by a instance of the 'FeatureVectorProducer' class.  The 
   * appropriate 'FeatureVectorProducer' derived class will be manufactured by a corresponding 'FactoryFVProducer' class.
   * This class(FactoryFVProducer) will maintain a list of all 'FactoryFVProducer' derived classes created in the 
   * 'factories' static data member.  
   *
   * The idea is for each class derived from FeatureVector there will be a corresponding 'FeatureVectorProducer'
   * and  'FactoryFVProducer'.  The 'FeatureVectorProducer' will be responsible for generating 'FeatureVector' instances
   * from source data(Images) and the 'FactoryFVProducer' class will generate the correct 'FactoryFVProducer' to do 
   * these computations.  Other functions of this class are to return the corresponding "FileDesc" instance, that describes
   * the features that are going to be computed, and the appropriate 'FeatureFectorList'  derived container.
   */
  class FactoryFVProducer
  {
  public:
    typedef  FactoryFVProducer*  FactoryFVProducerPtr;

    FactoryFVProducer (const KKStr&  _name,
                       const KKStr&  _fvClassName,
                       const KKStr&  _description
                      );

  protected:
    /**
     *@brief  A Factory can near be deleted until the application terminates;  the atexit method will perform the deletes.
     */
    virtual ~FactoryFVProducer ();

  public:
    const KKStr&   Description () const  {return description;}
    const KKStr&   FvClassName () const  {return fvClassName;}
    const KKStr&   Name        () const  {return name;}


    virtual  FeatureFileIOPtr  DefaultFeatureFileIO     ()  const = 0;

    /**  @brief  Returns the 'type_info' of the FeatureVector that this instance of 'FactoryFVProducer' works with. */
    virtual  const type_info*  FeatureVectorTypeId      () const = 0;

    /**  @brief  Returns the 'type_info' of the FeatureVectorList that this instance of 'FactoryFVProducer' will create. */
    virtual  const type_info*  FeatureVectorListTypeId  () const = 0;

    virtual  FileDescConstPtr       FileDesc            ()  const = 0;

    /**
     *@brief   Derived classes will instantiate appropriate instances of 'FeatureVectorProducer'.
     */
    virtual  FeatureVectorProducerPtr  ManufactureInstance (RunLog&  runLog)  = 0;


    /**
     *@brief Manufactures a instance of a derived 'FeatureVector' class that is appropriate for containing instances
     *of FeatureVectors produced by the associated FeatureVectorProducer.
     */
    virtual  FeatureVectorPtr  ManufacturFeatureVector (kkint32  numOfFeatires,
                                                        RunLog&  runLog
                                                       );

    /**
     *@brief Manufactures a instance of a derived 'FeatureVectorList' class that is appropriate for containing instances
     *of FeatureVectors produced by the associated FeatureVectorProducer.
     */
    virtual  FeatureVectorListPtr  ManufacturFeatureVectorList (bool owner) const;


    /**
     *@brief  Returns a 'TrainingConfiguration2'  derived instance.
     */
    virtual  TrainingConfiguration2Ptr  ManufacturTrainingConfiguration ()  const;


    /**
     *@brief Will instantiate a new instance of the appropriate 'FeatureVectorProducer' class given the supplied name.
     *@param[in]  name  The name given the 'FactoryFVProducer' instance when originally instantiated;  this will
     *  typically be the name of the 'FeatureVector' derived Class.
     *@param[in] runLog  reference to the logger where messages will be written to.
     *@returns returns a new instance of 'FeatureVectorProducer' that will be generated by registered 'FactoryFVProducer'; If 
     * 'name' is not found will return NULL.
     */
    static  FeatureVectorProducerPtr  ManufactureInstance (const KKStr&  name,
                                                           RunLog&       runLog
                                                          );


    /**
     *@brief  Returns pointer to existing instance of 'FactoryFVProducer' that was previously registered with 'name'.
     *@param[in]  name  The name of the 'FactoryFVProducer' that you are locking for; this is the same name as would
     * have been passed to the constructor.
     *@returns  The existing 'FactoryFVProducer' instance of found otherwise NULL.
     */
    static  FactoryFVProducerPtr  LookUpFactory (const KKStr&  name);



    /**
     *@brief  This is where you register newly instantiated instances of 'FactoryFVProducer' derived classes.
     *@details The instance will be added to a list of existing factories "FactoryFVProducer::factories".
     *This list is a map structure indexed by "FactoryFVProducer::Name()". If an instance with the same
     *name already exists a appropriate error message will be written to the "runLog" and will not be added
     * to "FactoryFVProducer::factories".
     *@param[in]  factory  The newly created instance of a "FactoryFVProducer" derived class that you want to register.
     *@param[in]  runLog  Pointer to logger; if equal NULL will write messages to temp created instance. 
     */
    static  void  RegisterFactory (FactoryFVProducerPtr  factory,
                                   RunLog*               runLog
                                  );


  private:
    KKStr   description;            /**<  Description  of  FeatuireVectorProducer.                                                            */
    KKStr   fvClassName;            /**<  The name of the 'FeatureVector' derived class that we are going to generate.                         */
    KKStr   name;                   /**<  Name of FeatuireVectorProducer will create.                                                         */

    static  void  FinaleCleanUp ();

    static  bool  atExitDefined;

    typedef  std::map<KKStr,FactoryFVProducerPtr>  FactoryMap;
    static  FactoryMap*  factories;
  };  /* FactoryFVProducer */

  typedef  FactoryFVProducer::FactoryFVProducerPtr  FactoryFVProducerPtr;

#define  _FactoryFVProducer_Defined_


}  /* KKMLL */



#endif
