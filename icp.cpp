#include "icp.h"

ICP::ICP (boost::shared_ptr< PCLPointCloud > cloud_input, boost::shared_ptr< PCLPointCloud > cloud_target, boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer)
{
	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_input0 (new pcl::PointCloud<pcl::PointXYZ>);
	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_target0 (new pcl::PointCloud<pcl::PointXYZ>);

	printf(" ok!\nDownsampling input cloud...");
	fflush(stdout);
	downsample(cloud_input, cloud_input0, 0.01);
	
	printf(" ok!\nRead target cloud...");
	printf(" ok!\nDownsampling target cloud...");
	fflush(stdout);
	downsample(cloud_target, cloud_target0, 0.01);
	printf(" ok!\n");
	
	pcl::IterativeClosestPoint<pcl::PointXYZ, pcl::PointXYZ> icp;
	icp.setMaximumIterations (1);
	icp.setMaxCorrespondenceDistance (0.2);
	icp.setInputCloud(cloud_input);
	icp.setInputTarget(cloud_target);
	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_output(new pcl::PointCloud<pcl::PointXYZ>);
	*cloud_output = *cloud_input;

	printf("Running ICP...");
	fflush(stdout);
	icp.align(*cloud_output);
	printf(" ok! (score %f)\n", (float)icp.getFitnessScore());
	
// 	pcl::PCDWriter writer;
// 	pcl::transformPointCloud(*cloud_input, *cloud_output, icp.getFinalTransformation());
// 	writer.writeASCII("result_icp.pcd", *cloud_output);

	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_outliers (new pcl::PointCloud<pcl::PointXYZ>);
	//pclGetOutliers(cloud_input, cloud_output, cloud_outliers, 0.1);
    
  pcl::SegmentDifferences<pcl::PointXYZ> sgmnt;
  sgmnt.setDistanceThreshold(0.1);
  sgmnt.setTargetCloud(cloud_target0);
  sgmnt.setInputCloud(cloud_output);
  sgmnt.segment(*cloud_outliers);
/*  
  std::cout<<"input: "<<cloud_input->size()<<std::endl;
  std::cout<<"output: "<<cloud_output->size()<<std::endl;
  std::cout<<"outliers: "<<cloud_outliers->size()<<std::endl;*/
// 	try{writer.writeASCII("outliers.pcd", *cloud_outliers);}catch(...){}

	//pclGetOutliers(cloud_input, cloud_outliers, cloud_input, 0.1);
// 	try{	writer.writeASCII("non_outliers.pcd", *cloud_input);}catch(...){}

	
	//Showing results
	int v3(0);
	viewer->createViewPort (0.0, 0.3333, 0.5, 0.6666, v3);
	viewer->addText ("Point clouds after ICP", 10, 10, "v3 text", v3);
	pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZ> color_target(cloud_target, 255, 0, 0);
	pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZ> color_output(cloud_output, 0, 255, 0);
	viewer->addPointCloud<pcl::PointXYZ> (cloud_target, color_target, "icp_target", v3);
	viewer->addPointCloud<pcl::PointXYZ> (cloud_output, color_output, "icp_output", v3);
	
	//Showing outliers
	int v4(0);
	viewer->createViewPort (0.5, 0.3333, 1.0, 0.6666, v4);
	viewer->addText ("ICP outliers", 10, 10, "v4 text", v4);	
	pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZ> color_outliers(cloud_outliers, 0, 0, 255);
	viewer->addPointCloud<pcl::PointXYZ> (cloud_outliers, color_outliers, "icp_outliers", v4);
	
}

void ICP::downsample(const boost::shared_ptr< pcl::PointCloud<pcl::PointXYZ> > &input, const boost::shared_ptr< pcl::PointCloud<pcl::PointXYZ> > &output, float grid_size)
{
	static pcl::PointCloud<pcl::PointXYZ> cloud_downsampled;
	pcl::VoxelGrid<pcl::PointXYZ> sor;
	sor.setLeafSize(grid_size, grid_size, grid_size);
	if (input.get() == output.get())
	{
		sor.setInputCloud(input);
		sor.filter(cloud_downsampled);
		*output = cloud_downsampled;
	}
	else
	{
		sor.setInputCloud(input);
		sor.filter(*output);
	}
}

void ICP::pclGetOutliers(const boost::shared_ptr< pcl::PointCloud<pcl::PointXYZ> > base, const boost::shared_ptr< pcl::PointCloud<pcl::PointXYZ> > cloud, boost::shared_ptr< pcl::PointCloud<pcl::PointXYZ> > outliers, double threshold)
{
	pcl::SegmentDifferences<pcl::PointXYZ> sgmnt;
	sgmnt.setDistanceThreshold(threshold);
	sgmnt.setTargetCloud(base);
	sgmnt.setInputCloud(cloud);
	sgmnt.segment(*outliers);
}

