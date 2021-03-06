#ifndef  _MODEL_
#define  _MODEL_
/**
 @class  KKMLL::Model
 @brief Base class to all Learning Algorithms.
 @author  Kurt Kramer
 @details
   Base class to be used by all Learning Algorithm Models. The idea is that all learning algorithms 
   all follow the same basic flow.  The two obvious functions that they all support are Training from 
   supplied labeled examples(List of FeatureVector objects),  Prediction of an unlabeled example.
 */

#include "DateTime.h"
#include "KKBaseTypes.h"
#include "KKStr.h"
#include "RunLog.h"

#include "ModelParam.h"

namespace KKMLL
{
  #if  !defined(_CLASSPROB_)
  class  ClassProb;
  typedef  ClassProb*  ClassProbPtr;
  class  ClassProbList;
  typedef  ClassProbList*  ClassProbListPtr;
  #endif

  #if  !defined(_FEATUREENCODER2_)
  class  FeatureEncoder2;
  typedef  FeatureEncoder2*  FeatureEncoder2Ptr;
  #endif


  #ifndef  _FeatureNumList_Defined_
  class  FeatureNumList;
  typedef  FeatureNumList*  FeatureNumListPtr;
  typedef  FeatureNumList const  FeatureNumListConst;
  typedef  FeatureNumListConst*  FeatureNumListConstPtr;
  #endif


  #if  !defined(_FEATUREVECTOR_)
  class  FeatureVector;
  typedef  FeatureVector*  FeatureVectorPtr;
  class  FeatureVectorList;
  typedef  FeatureVectorList*  FeatureVectorListPtr;
  #endif


  #if  !defined(_FileDesc_Defined_)
  class  FileDesc;
  typedef  FileDesc*  FileDescPtr;
  #endif


  #if  !defined(_MLCLASS_)
  class  MLClass;
  typedef  MLClass*  MLClassPtr;
  class  MLClassList;
  typedef  MLClassList*  MLClassListPtr;
  class  MLClassIndexList;
  typedef  MLClassIndexList*  MLClassIndexListPtr;
  #endif


  #if  !defined(_NORMALIZATIONPARMS_)
  class  NormalizationParms;
  typedef  NormalizationParms*  NormalizationParmsPtr;
  #endif


  #if  !defined(_FactoryFVProducer_Defined_)
  class  FactoryFVProducer;
  typedef  FactoryFVProducer*  FactoryFVProducerPtr;
  #endif
  

  class  Model
  {
  public:
    typedef  Model*  ModelPtr;

    enum  class  ModelTypes: int 
    {
      Null      = 0,
      OldSVM    = 1,
      SvmBase   = 2,
      KNN       = 3,
      UsfCasCor = 4,
      Dual      = 5
    };

    static KKStr       ModelTypeToStr   (ModelTypes    _modelingType);
    static ModelTypes  ModelTypeFromStr (const KKStr&  _modelingTypeStr);


    Model ();

    /**
     *@brief  Use this when you are planning on creating a empty model without parameters.
     */
    Model (FactoryFVProducerPtr  _factoryFVProducer);


    /**
     *@brief  Construct a instance of 'Model' using the parameters specified in '_param'.
     *@param[in]  _name Name of training model.
     *@param[in]  _param  Parameters for learning algorithm; we will create a duplicate copy.
     *@param[in]  _fileDesc Description of the dataset that will be used to train the classifier and examples that will be classified.
     *@param[in]  _cancelFlag  Will monitor; if at any point it turns true this instance is to terminate and return to caller.
     *@param[in,out]  _log  Logging file.
     */
    Model (const KKStr&          _name,
           const ModelParam&     _param,         // Create new model from
           FactoryFVProducerPtr  _factoryFVProducer
          );

  
    /**
     *@brief Copy Constructor.
     */
    Model (const Model&   _madel);

    virtual  ~Model ();



    /**
     *@brief  A factory method that will instantiate the appropriate class of training model based off '_modelType'.
     *@details  This method is used to construct a model that is going to be built from training data.
     *@param[in] _modelType  Type of model to be created; ex: mtOldSVM, mtSvmBase, or  mtKNN.
     *@param[in] _name
     *@param[in] _param  Parameters used to drive the creating of the model.
     *@param[in] _fileDesc Description of the dataset that will be used to train the classifier and examples that will be classified.
     *@param[in] _cancelFlag  Will monitor; if at any point it turns true this instance is to terminate and return to caller.
     *@param[in,out]  _log  Logging file.
     */
    static 
      ModelPtr  CreateAModel (ModelTypes            _modelType,
                              const KKStr&          _name,
                              const ModelParam&     _param,      /**< Will make a duplicate copy of */
                              FactoryFVProducerPtr  _factoryFVProducer,
                              VolConstBool&         _cancelFlag,
                              RunLog&               _log
                             );
  

    virtual
    ModelPtr                 Duplicate () const = 0;


    // Access Methods
    bool                              AlreadyNormalized          () const {return alreadyNormalized;}

    virtual
    KKStr                             Description                () const;  /**< Return short user readable description of model. */

    const FeatureEncoder2&            Encoder                    () const;

    FactoryFVProducerPtr              FactoryFVProducer          () const  {return factoryFVProducer;}

    virtual FeatureNumListConstPtr    GetFeatureNums             () const;

    virtual kkMemSize                 MemoryConsumedEstimated    () const;

    MLClassListPtr                    MLClasses                  () const  {return  classes;}

    MLClassListPtr                    MLClassesNewInstance       () const;  /**< Returns a new instances of 'classes' by calling copy constructor. */

    virtual ModelTypes                ModelType                  () const = 0;

    virtual KKStr                     ModelTypeStr               () const  {return ModelTypeToStr (ModelType ());}

    const KKStr&                      Name                       () const  {return name;}
    void                              Name (const KKStr&  _name)  {name = _name;}

    virtual bool                      NormalizeNominalAttributes () const; /**< Return true, if nominal fields need to be normalized. */

    ModelParamPtr                     Param                      () const  {return  param;}

    virtual FeatureNumListConstPtr    SelectedFeatures           () const;

    const KKStr&                      RootFileName               () const {return rootFileName;}

    const KKB::DateTime&              TimeSaved                  () const {return timeSaved;}

    double                            TrainingTime               () const {return trainingTime;}

    double                            TrianingPrepTime           () const {return trianingPrepTime;}  //*< Time ins sec's spent preparing training data in Model::TrainModel */

    bool                              ValidModel                 () const {return validModel;}


    // Access Update Methods
    void  RootFileName (const KKStr&  _rootFileName)  {rootFileName = _rootFileName;}
  


    /**
     @brief  Derived classes call this method to start the clock for 'trainingTime'.
     */
    void  TrainingTimeStart ();

    /**
     @brief  Derived classes call this method to stop the clock for 'trainingTime'.
     */
    void  TrainingTimeEnd ();



    /**
     *@brief  Every prediction  method in every class that is inherited from this class should call
     *        this method before performing there prediction.  Such things as Normalization and
     *        Feature Encoding.
     *@param[in]  fv  Feature vector of example that needs to be prepared.
     *@param[out]  newExampleCreated  Indicates if either Feature Encoding and/or Normalization needed
     *             to be done.  If neither then the original instance is returned.  If Yes then 
     *             a new instance which the caller will have to delete will be returned.
     */
    virtual
    FeatureVectorPtr  PrepExampleForPrediction (FeatureVectorPtr  fv,
                                                bool&             newExampleCreated
                                               );


    virtual  void  PredictRaw (FeatureVectorPtr  example,
                               MLClassPtr&       predClass,
                               double&           dist
                              );

    //*********************************************************************
    //*     Routines that should be implemented by descendant classes.    *
    //*********************************************************************

    virtual
    MLClassPtr  Predict (FeatureVectorPtr  example,
                         RunLog&           log
                        ) = 0;
  
    virtual
    void        Predict (FeatureVectorPtr  example,
                         MLClassPtr        knownClass,
                         MLClassPtr&       predClass1,
                         MLClassPtr&       predClass2,
                         kkint32&          predClass1Votes,
                         kkint32&          predClass2Votes,
                         double&           probOfKnownClass,
                         double&           predClass1Prob,
                         double&           predClass2Prob,
                         kkint32&          numOfWinners,
                         bool&             knownClassOneOfTheWinners,
                         double&           breakTie,
                         RunLog&           log
                        ) = 0;


   virtual 
   ClassProbListPtr  ProbabilitiesByClass (FeatureVectorPtr  example,
                                           RunLog&           log
                                          ) = 0;


    /**@brief  Only applied to ModelDual classifier. */
    virtual
    void  ProbabilitiesByClassDual (FeatureVectorPtr   example,
                                    KKStr&             classifier1Desc,
                                    KKStr&             classifier2Desc,
                                    ClassProbListPtr&  classifier1Results,
                                    ClassProbListPtr&  classifier2Results,
                                    RunLog&            log
                                   );


    virtual
    void  ProbabilitiesByClass (FeatureVectorPtr    example,
                                const MLClassList&  _mlClasses,
                                kkint32*            _votes,
                                double*             _probabilities,
                                RunLog&             _log
                               ) = 0;

    /**
     *@brief Derives predicted probabilities by class.
     *@details Will get the probabilities assigned to each class by the classifier.  The 
     *        '_mlClasses' parameter dictates the order of the classes. That is the 
     *        probabilities for any given index in '_probabilities' will be for the class
     *        specified in the same index in '_mlClasses'.
     *@param[in]  _example       FeatureVector object to calculate predicted probabilities for.
     *@param[in]  _mlClasses  List of classes that caller is aware of. This should be the
     *            same list that was used when constructing this Model object.  The list must
     *            be the same but not necessarily in the same order as when Model was 1st
     *            constructed.  The ordering of this list will dictate the order that '_probabilities'
     *            will be populated.
     *@param[out] _probabilities An array that must be as big as the number of classes in
     *            '_mlClasses'.  The probability of class in '_mlClasses[x]' will be 
     *            returned in probabilities[x].
     */
    virtual
    void  ProbabilitiesByClass (FeatureVectorPtr    _example,
                                const MLClassList&  _mlClasses,
                                double*             _probabilities,
                                RunLog&             _log
                               ) = 0;
  

    virtual  
    void  RetrieveCrossProbTable (MLClassList&  _classes,
                                  double**      _crossProbTable,  /**< two dimension matrix that needs to be classes.QueueSize ()  squared. */
                                  RunLog&       log
                                 );

    /**
     *@brief Performs operations such as FeatureEncoding, and  Normalization.  The actual training
     *  of models occurs in the specific derived implementation of 'Model'.
     *@param[in] _trainExamples  Training data that classifier will be built from.  If the examples need to be
     *                           normalized or encoded and we are not taking ownership then a duplicate list of
     *                           examples will be created that this method and class will be free to modify.
     *@param[in] _alreadyNormalized  Indicates if contents of '_trainExamples' are normalized already; if not
     *                               they will be normalized.
     *@param[in] _takeOwnership  This instance of Model will take ownership of '_examples' and is free to 
     *                           modify its contents.
     */
    virtual  
    void  TrainModel (FeatureVectorListPtr  _trainExamples,
                      bool                  _alreadyNormalized,
                      bool                  _takeOwnership,
                      VolConstBool&         _cancelFlag,
                      RunLog&               _log
                     );


    /**
     *@brief  To be implemented by derived classes;  the parent classes fields will be updated by the 
     * derived class calling ReadXMLModelToken.  
     */
    virtual  void  ReadXML (XmlStream&      s,
                            XmlTagConstPtr  tag,
                            VolConstBool&   cancelFlag,
                            RunLog&         log
                           ) = 0;


    virtual  void  WriteXML (const KKStr&   varName,
                             std::ostream&  o
                            )  const = 0;


    /**
     *@brief  The "WriteXML" method in Derived classes call this method to include the parents classes fields in the XML data.
     */
    void  WriteModelXMLFields (std::ostream&  o)  const;

  protected:
    void  AddErrorMsg (const KKStr&  errMsg,
                       kkint32       lineNum
                      );

    void  AllocatePredictionVariables ();


    void  DeAllocateSpace ();


    void  NormalizeProbabilitiesWithAMinumum (kkint32  numClasses,
                                              double*  probabilities,
                                              double   minProbability
                                             );

    /**  @brief  Will process any tokens that belong to 'ModelParam' and return NULL ones that are not will be passed back. */
    XmlTokenPtr  ReadXMLModelToken (XmlTokenPtr  t,
                                    RunLog&      log
                                   );

    void  ReadXMLModelPost (RunLog&  log);

    void  ReduceTrainExamples (RunLog&  log);



    bool                   alreadyNormalized;

    MLClassListPtr         classes;

    MLClassIndexListPtr    classesIndex;

    double*                classProbs;
 
    double**               crossClassProbTable;   /*!< Probabilities  between Binary Classes From last Prediction */

    kkuint32               crossClassProbTableSize;

    FeatureEncoder2Ptr     encoder;

    VectorKKStr            errors;

    FactoryFVProducerPtr   factoryFVProducer;

    FileDescConstPtr       fileDesc;

    NormalizationParmsPtr  normParms;

    kkuint32               numOfClasses;   /**< Number of Classes defined in crossClassProbTable. */

    ModelParamPtr          param;          /**< Will own this instance,                           */

    KKStr                  rootFileName;   /**< This is the root name to be used by all component objects; such as svm_model,
                                            * mlClasses, and svmParam(including selected features). Each one will have the
                                            * same rootName with a different suffix.
                                            *@code
                                            *      mlClasses  "<rootName>.classes"
                                            *      svmParam      "<rootName>.svm_parm"
                                            *      model         "<rootName>"
                                            *@endcode
                                            */

    FeatureVectorListPtr   trainExamples;

    bool                   validModel;

    kkint32*               votes;

    bool                   weOwnTrainExamples;  /**< Indicates if we own the 'trainExamples'. This does not mean that we own its
                                                 * contents. That is determined by 'trainExamples->Owner ()'.
                                                 */
   
  private:
    double                 trianingPrepTime;    /**<  Time that it takes to perform normalization, and encoding */
    double                 trainingTime;
    double                 trainingTimeStart;   /**<  Time that the clock for TraininTime was started. */
    KKStr                  name;
    KKB::DateTime          timeSaved;           /**<  Date and Time that this model was saved. */
  };  /* Model */
  
  typedef  Model::ModelPtr  ModelPtr;

#define  _Model_Defined_

  
  
  /**
   *@brief  The base class to be used for the manufacturing if "Model" derived classes.
   */
  class  XmlElementModel:  public  XmlElement
  {
  public:
    XmlElementModel (XmlTagPtr   tag,
                     XmlStream&  s,
                     RunLog&     log
                    ):
        XmlElement (tag, s, log)
    {}
                
    virtual  ~XmlElementModel ()
    {
      delete  value;
      value = NULL;
    }

    ModelPtr  Value ()  const   {return value;}

    ModelPtr  TakeOwnership ()
    {
      ModelPtr v = value;
      value = NULL;
      return v;
    }

  protected:
    ModelPtr  value;
  };
  typedef  XmlElementModel*  XmlElementModelPtr;



  template<class ModelType>
  class  XmlElementModelTemplate:  public  XmlElementModel
  {
  public:
    XmlElementModelTemplate (XmlTagPtr      tag,
                             XmlStream&     s,
                             VolConstBool&  cancelFlag,
                             RunLog&        log
                            ):
      XmlElementModel (tag, s, log)
    {
      value = new ModelType();
      value->ReadXML (s, tag, cancelFlag, log);
    }
                
    virtual  ~XmlElementModelTemplate ()
    {
    }

    ModelType*  Value ()  const   {return dynamic_cast<ModelType*>(value);}


    ModelType*  TakeOwnership ()  {return dynamic_cast<ModelType*> (XmlElementModel::TakeOwnership ());}
  };  /* XmlElementModelTemplate */

  

}  /* namespace MML */

#endif




